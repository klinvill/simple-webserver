#include <strings.h>     /* for bzero, bcopy */
#include <unistd.h>      /* for read, write */
#include <sys/socket.h>  /* for socket use */
#include <netinet/in.h>
#include <thread>
#include <sstream>
#include <iostream>
#include <fstream>
#include <sys/stat.h>

#include "http_message.h"


#define MAXBUF   8192  /* max I/O buffer size */
#define LISTENQ  1024  /* second argument to listen() */

int open_listenfd(int port);
void process_request(int connfd);
void handle_connection(void * vargp);
void handle_get(const HttpRequestMessage& message, int connfd);
void handle_post(const HttpRequestMessage& message, int connfd);

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
    process_request(connfd);
    close(connfd);
}

void send_response(HttpResponseMessage& response, int connfd) {
    std::string response_string = std::string(response);
    write(connfd, response_string.c_str(), response_string.length());
}

void send_error(int connfd) {
    HttpResponseMessage response(500, "Internal Server Error", ContentType::txt, std::string());
    send_response(response, connfd);
}

void process_request(int connfd)
{
    size_t n;
    char buf[MAXBUF];

    n = read(connfd, buf, MAXBUF);
    printf("server received the following request:\n%s\n",buf);

    HttpRequestMessage request = HttpRequestMessage(buf);

    if (request.header.type == RequestTypeEnum::GET)
        handle_get(request, connfd);
    else
        send_error(connfd);
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

void handle_get(const HttpRequestMessage& message, int connfd) {
    // TODO: should the content directory be specified? Or default to www?
    std::string relative_resource = join_filepath("www/", message.header.resource);
    std::ifstream ifs;

    std::string filename = relative_resource;
    if (is_file(relative_resource))
        ifs.open(relative_resource, std::ifstream::in);
    else {
        try {
            ifs = open_default(relative_resource);
        } catch (const std::runtime_error &err) {
            std::cerr << err.what();
            send_error(connfd);
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

    HttpResponseMessage response(200, "OK", from_filename(filename), response_message);
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
