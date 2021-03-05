#include <strings.h>     /* for bzero, bcopy */
#include <unistd.h>      /* for read, write */
#include <sys/socket.h>  /* for socket use */
#include <netinet/in.h>
#include <thread>
#include <sstream>
#include <iostream>
#include <fstream>
#include <poll.h>

#include "http_message.h"
#include "file_helpers.h"


#define MAXBUF   8192  /* max I/O buffer size */
#define LISTENQ  1024  /* second argument to listen() */
#define CONNECTION_TIMEOUT  10000     // milliseconds to wait before closing a Keep-Alive connection

#define CONTENT_ROOT "www/"     // Content directory rooted at www/

int open_listenfd(int port);
bool process_request(char* buf, int connfd, bool prev_keepalive);
void handle_connection(void * vargp);
void handle_get(const HttpRequestMessage& message, int connfd, ConnectionDirective connection_directive);
void handle_post(const HttpRequestMessage& message, int connfd, ConnectionDirective connection_directive);

int main(int argc, char **argv)
{
    int listenfd, *connfdp, port;
    socklen_t clientlen = sizeof(struct sockaddr_in);
    struct sockaddr_in clientaddr {};

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    port = strtol(argv[1], nullptr, 10);

    listenfd = open_listenfd(port);
    while (true) {
        // spawns a thread to handle each new connection
        connfdp = static_cast<int *>(malloc(sizeof(int)));
        *connfdp = accept(listenfd, (struct sockaddr*)&clientaddr, &clientlen);
        std::thread tmp_thread(handle_connection, connfdp);
        tmp_thread.detach();
    }
}

// thread routine to handle a connection
// keeps the connection open if the request specifies keep-alive
void handle_connection(void * vargp)
{
    int connfd = *((int *)vargp);
    free(vargp);

    size_t n;
    char buf[MAXBUF];

    struct pollfd poll_fds[1];
    poll_fds[0] = (struct pollfd) {.fd=connfd, .events=POLLIN};
    // default to not keeping connections alive unless specifically given a keep-alive header
    bool keepalive = false;

    do {
        n = read(connfd, buf, MAXBUF);
        // error or connection closed on other end
        if (n <= 0)
            break;
        // ensure string is null-terminated
        buf[(n < MAXBUF-1) ? n : MAXBUF-1] = 0;

        keepalive = process_request(buf, connfd, keepalive);
    } while(keepalive && poll(poll_fds, 1, CONNECTION_TIMEOUT) > 0);

    std::cout << "Closing connection...\n";
    std::flush(std::cout);
    close(connfd);
}

// sends a HTTP response to the specified connection
void send_response(HttpResponseMessage& response, int connfd) {
    std::string response_string = std::string(response);
    write(connfd, response_string.c_str(), response_string.length());
}

// sends a HTTP 500 error message to the specified connection
void send_error(int connfd, ConnectionDirective connection_directive) {
    HttpResponseMessage response(500, "Internal Server Error", ContentType::txt, std::string(),
                                 HttpVersion(HttpVersionEnum::HTTP_1_1), connection_directive);
    send_response(response, connfd);
}

// Processes an HTTP request read as a string
// Returns whether the connection should be kept alive
bool process_request(char* buf, int connfd, bool prev_keepalive)
{
    // preserve keepalive value unless given an explicit keep-alive or close directive
    bool keepalive = prev_keepalive;
    printf("server received the following request:\n%s\n",buf);

    // the HttpRequestMessage parsing can throw an invalid_argument exception, the request handling can throw
    // runtime_exceptions as well
    try {
        HttpRequestMessage request = HttpRequestMessage(buf);
        ConnectionDirective connection_directive = request.header.connection_directive;

        if (connection_directive.directive == ConnectionDirectiveEnum::KEEP_ALIVE) {
            keepalive = true;
        } else if (connection_directive.directive == ConnectionDirectiveEnum::CLOSE) {
            keepalive = false;
        } else if (connection_directive.directive == ConnectionDirectiveEnum::EMPTY) {
            // If no connection header is specified, we respond with a header that is consistent with the preserved
            // keep-alive value. This means we always send explicit keep-alive headers when we keep a connection alive
            connection_directive = ConnectionDirective(keepalive ? "keep-alive" : "close");
        }

        if (request.header.type == RequestTypeEnum::GET)
            handle_get(request, connfd, connection_directive);
        else if (request.header.type == RequestTypeEnum::POST)
            handle_post(request, connfd, connection_directive);
        else
            send_error(connfd, connection_directive);
    }
    // in the case of any error parsing or handling the request, we send a HTTP 500 message back
    catch (const std::exception &err) {
        std::cerr << err.what();
        send_error(connfd, ConnectionDirective());
        return keepalive;
    }

    return keepalive;
}

// Checks to see if index.html or index.htm are present in the given directory. If so, it returns the path to the found
// file.
//
// Throws runtime_error
std::string find_default_file(const std::string& directory) {
    std::string filepath = join_filepath(directory, "index.html");
    std::ifstream dot_html_ifs(filepath, std::ifstream::in);
    if (!dot_html_ifs.fail())
        return filepath;
    std::cerr << "Could not open " << filepath << "\n";
    dot_html_ifs.close();

    filepath = join_filepath(directory, "index.htm");
    std::ifstream dot_htm_ifs(filepath, std::ifstream::in);
    if (!dot_htm_ifs.fail())
        return filepath;
    std::cerr << "Could not open " << filepath << "\n";
    dot_htm_ifs.close();

    throw std::runtime_error("Could not find a file to open");
}

// Similar to find_default_file(), but only checks if the resource is a file, and if not returns index.html
// This is useful for the POST method which is only supported for .html files
std::string get_filepath(const std::string& resource) {
    if (is_file(resource))
        return resource;
    else
        return join_filepath(resource, "index.html");
}

// handles HTTP GET requests by opening the requested resource and sending it back as an HTTP response
void handle_get(const HttpRequestMessage& message, int connfd, ConnectionDirective connection_directive) {
    // TODO: this allows for path traversal attacks, should fix
    std::string relative_resource = join_filepath(CONTENT_ROOT, message.header.resource);
    std::ifstream ifs;

    // tries to open the resource for reading
    std::string filepath = relative_resource;
    if (is_file(relative_resource))
        ifs.open(relative_resource, std::ifstream::in);
    else {
        try {
            filepath = find_default_file(relative_resource);
            ifs.open(filepath, std::ifstream::in);
        } catch (const std::runtime_error &err) {
            std::cerr << err.what();
            send_error(connfd, connection_directive);
            return;
        }
    }

    std::string response_message;

    // pre-allocate buffer for performance
    ifs.seekg(0, ifs.end);
    response_message.reserve(ifs.tellg());
    ifs.seekg(0, ifs.beg);

    response_message.assign(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());

    HttpResponseMessage response(200, "OK", from_filename(filepath), response_message,
                                 message.header.version, connection_directive);
    send_response(response, connfd);

    ifs.close();
}

// handles HTTP POST requests by opening the requested resource (if it refers to a .html file) and sending it back as
// an HTTP response prepended with the string "<h1>POST DATA</h1><pre>${data_from_post_request}</pre>" where
// "${data_from_post_request}" refers to the data in the post request.
void handle_post(const HttpRequestMessage& message, int connfd, ConnectionDirective connection_directive) {
    // POST requests are only supported for .html files as per the homework instructions
    std::string resource = join_filepath(CONTENT_ROOT, message.header.resource);
    std::string filepath = get_filepath(resource);
    if (get_extension(filepath) != "html") {
        send_error(connfd, connection_directive);
        return;
    }

    std::string post_content_prefix = "<h1>POST DATA</h1><pre>";
    std::string post_content_suffix = "</pre>";

    std::ifstream ifs;

    if (!is_file(filepath)) {
        send_error(connfd, connection_directive);
        return;
    }

    ifs.open(filepath, std::ifstream::in);

    std::string file_contents;

    // pre-allocate buffer for performance
    ifs.seekg(0, ifs.end);
    file_contents.reserve(ifs.tellg());
    ifs.seekg(0, ifs.beg);

    file_contents.assign(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());

    // sends the file contents prepended with the post data string
    HttpResponseMessage response(200, "OK", from_filename(filepath),
                                 post_content_prefix + message.content + post_content_suffix + file_contents,
                                 message.header.version, connection_directive);
    send_response(response, connfd);

    ifs.close();
}

/*
 * open_listenfd - open and return a listening socket on port
 * Returns -1 in case of failure
 */
int open_listenfd(int port)
{
    int listenfd, optval=1;
    struct sockaddr_in serveraddr {};

    /* Create a socket descriptor */
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return -1;

    /* Eliminates "Address already in use" error from bind. */
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
                   (const void *)&optval , sizeof(int)) < 0)
        return -1;

    /* listenfd will be an endpoint for all requests to port
       on any IP address for this host */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short)port);
    if (bind(listenfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0)
        return -1;

    /* Make it a listening socket ready to accept connection requests */
    if (listen(listenfd, LISTENQ) < 0)
        return -1;
    return listenfd;
} /* end open_listenfd */
