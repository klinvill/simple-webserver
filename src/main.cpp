#include <strings.h>     /* for bzero, bcopy */
#include <unistd.h>      /* for read, write */
#include <sys/socket.h>  /* for socket use */
#include <netinet/in.h>
#include <thread>
#include <sstream>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
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
        connfdp = static_cast<int *>(malloc(sizeof(int)));
        *connfdp = accept(listenfd, (struct sockaddr*)&clientaddr, &clientlen);
        std::thread tmp_thread(handle_connection, connfdp);
        tmp_thread.detach();
    }
}

/* thread routine */
void handle_connection(void * vargp)
{
    int connfd = *((int *)vargp);
    free(vargp);

    size_t n;
    char buf[MAXBUF];

    struct pollfd poll_fds[1];
    poll_fds[0] = (struct pollfd) {.fd=connfd, .events=POLLIN};
    bool keepalive = false;

    do {
        n = read(connfd, buf, MAXBUF);
        // error or connection closed on other end
        if (n <= 0)
            break;
        buf[(n < MAXBUF-1) ? n : MAXBUF-1] = 0;

        keepalive = process_request(buf, connfd, keepalive);
    } while(keepalive && poll(poll_fds, 1, CONNECTION_TIMEOUT) > 0);

    std::cout << "Closing connection...\n";
    std::flush(std::cout);
    close(connfd);
}

void send_response(HttpResponseMessage& response, int connfd) {
    std::string response_string = std::string(response);
    write(connfd, response_string.c_str(), response_string.length());
}

void send_error(int connfd, ConnectionDirective connection_directive) {
    HttpResponseMessage response(500, "Internal Server Error", ContentType::txt, std::string(),
                                 HttpVersion(HttpVersionEnum::HTTP_1_1), connection_directive);
    send_response(response, connfd);
}

// returns whether the connection should be kept alive
bool process_request(char* buf, int connfd, bool prev_keepalive)
{
    // TODO: should we preserve the value?
    // preserve keepalive value unless given an explicit keep-alive or close directive
    bool keepalive = prev_keepalive;
    printf("server received the following request:\n%s\n",buf);

    try {
        HttpRequestMessage request = HttpRequestMessage(buf);
        // TODO: should we send Keep-alive headers back if a request doesn't set them but a previous one did?
        // TODO: need to send with "Connection: Close" header if no keep-alive header is initially received
        ConnectionDirective connection_directive = request.header.connection_directive;

        if (connection_directive.directive == ConnectionDirectiveEnum::KEEP_ALIVE) {
            keepalive = true;
        } else if (connection_directive.directive == ConnectionDirectiveEnum::CLOSE) {
            keepalive = false;
        }

        if (request.header.type == RequestTypeEnum::GET)
            handle_get(request, connfd, connection_directive);
        else if (request.header.type == RequestTypeEnum::POST)
            handle_post(request, connfd, connection_directive);
        else
            send_error(connfd, connection_directive);
    } catch (const std::exception &err) {
        std::cerr << err.what();
        send_error(connfd, ConnectionDirective());
        return keepalive;
    }

    return keepalive;
}

std::string join_filepath(const std::string& dir, const std::string& file) {
    std::stringstream joined_path;
    joined_path << dir;

    if (dir.back() != '/')
        joined_path << '/';

    if (file.front() == '/')
        joined_path << file.substr(1);
    else
        joined_path << file;

    return joined_path.str();
}

// Throws runtime_error
std::ifstream open_default(const std::string& directory) {
    std::ifstream dot_html_ifs(join_filepath(directory, "index.html"), std::ifstream::in);
    if (!dot_html_ifs.fail())
        return dot_html_ifs;
    std::cerr << "Could not open " << join_filepath(directory, "index.html") << "\n";
    dot_html_ifs.close();

    std::ifstream dot_htm_ifs(join_filepath(directory, "index.htm"), std::ifstream::in);
    if (!dot_htm_ifs.fail())
        return dot_htm_ifs;
    std::cerr << "Could not open " << join_filepath(directory, "index.htm") << "\n";
    dot_htm_ifs.close();

    throw std::runtime_error("Could not find a file to open");
}

bool is_file(const std::string& filepath) {
    struct stat stat_info;
    stat(filepath.c_str(), &stat_info);

    return S_ISREG(stat_info.st_mode);
}

long get_file_size(const std::string& filepath) {
    struct stat stat_info;
    stat(filepath.c_str(), &stat_info);

    return stat_info.st_size;
}

void handle_get(const HttpRequestMessage& message, int connfd, ConnectionDirective connection_directive) {
    // TODO: this allows for path traversal attacks, should fix
    std::string relative_resource = join_filepath(CONTENT_ROOT, message.header.resource);
    std::ifstream ifs;

    std::string filename = relative_resource;
    if (is_file(relative_resource))
        ifs.open(relative_resource, std::ifstream::in);
    else {
        try {
            ifs = open_default(relative_resource);
        } catch (const std::runtime_error &err) {
            std::cerr << err.what();
            send_error(connfd, connection_directive);
            return;
        }
        filename = "index.html";
    }

    std::string response_message;

    // pre-allocate buffer
    ifs.seekg(0, ifs.end);
    response_message.reserve(ifs.tellg());
    ifs.seekg(0, ifs.beg);

    response_message.assign(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());

    HttpResponseMessage response(200, "OK", from_filename(filename), response_message,
                                 message.header.version, connection_directive);
    send_response(response, connfd);

    ifs.close();
}

void handle_post(const HttpRequestMessage& message, int connfd, ConnectionDirective connection_directive) {
    // POST requests are only supported for .html files as per the homework instructions
    if (get_extension(message.header.resource) != "html") {
        send_error(connfd, connection_directive);
        return;
    }

    std::string post_content_prefix = "<h1>POST DATA</h1><pre>";
    std::string post_content_suffix = "</pre>";

    std::string relative_resource = join_filepath(CONTENT_ROOT, message.header.resource);
    std::ifstream ifs;

    if (!is_file(relative_resource)) {
        send_error(connfd, connection_directive);
        return;
    }

    ifs.open(relative_resource, std::ifstream::in);

    std::string file_contents;

    // pre-allocate buffer
    ifs.seekg(0, ifs.end);
    file_contents.reserve(ifs.tellg());
    ifs.seekg(0, ifs.beg);

    file_contents.assign(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());

    // TODO: confirm that we should just put the post content before the loaded html file (as stated in the assignment)
    HttpResponseMessage response(200, "OK", from_filename(relative_resource),
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
