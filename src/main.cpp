#include <strings.h>     /* for bzero, bcopy */
#include <unistd.h>      /* for read, write */
#include <sys/socket.h>  /* for socket use */
#include <netinet/in.h>
#include <thread>

#define MAXBUF   8192  /* max I/O buffer size */
#define LISTENQ  1024  /* second argument to listen() */

int open_listenfd(int port);
void echo(int connfd);
void handle_connection(void * vargp);

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
    echo(connfd);
    close(connfd);
}

/*
 * echo - read and echo text lines until client closes connection
 */
void echo(int connfd)
{
    size_t n;
    char buf[MAXBUF];
    char httpmsg[MAXBUF] = "HTTP/1.1 200 Document Follows\r\nContent-Type:text/html\r\nContent-Length:32\r\n\r\n<html><h1>Hello CSCI4273 Course!</h1></html>";

    n = read(connfd, buf, MAXBUF);
    printf("server received the following request:\n%s\n",buf);
    strncpy(buf, httpmsg, strlen(httpmsg));
    buf[MAXBUF-1] = 0;
    printf("server returning a http message with the following content.\n%s\n",buf);
    write(connfd, buf, strlen(buf));
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
