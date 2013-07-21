/* author: pjankows@gmail.com */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define ACCEPT_BUFQ 20

struct addrinfo* prepare_addrinfo(char *host, char *port)
{
    int status;
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if (host == NULL) {
        hints.ai_flags = AI_PASSIVE;
    }
    status = getaddrinfo(host, port, &hints, &res);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        res = NULL;
    }
    return res;
}

int get_socket(struct addrinfo *res)
{
    int s = -1;
    if (res != NULL) {
        s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    }
    return s;
}

int get_port_and_ip(struct addrinfo *res, char *ip)
{
    void *addr;
    unsigned short int port;
    if (res->ai_family == AF_INET) {
        port = ((struct sockaddr_in*)res->ai_addr)->sin_port;
        addr = &((struct sockaddr_in*)res->ai_addr)->sin_addr;
    }
    else {
        port = ((struct sockaddr_in6*)res->ai_addr)->sin6_port;
        addr = &((struct sockaddr_in6*)res->ai_addr)->sin6_addr;
    }
    inet_ntop(res->ai_family, addr, ip, INET6_ADDRSTRLEN);
    return ntohs(port);
}

int bind_socket(struct addrinfo *res)
{
    int sfd, b;
    int port;
    char ip[INET6_ADDRSTRLEN];
    sfd = get_socket(res);
    if (sfd != -1) {
        b = bind(sfd, res->ai_addr, res->ai_addrlen);
        port = get_port_and_ip(res, ip);
        if (b != 0) {
            fprintf(stderr, "ERROR: bind to port %d on %s failed: ", port, ip);
            perror(NULL);
            close(sfd);
            sfd = -1;
        }
        else {
            printf("bind to port %d on %s OK\n", port, ip);
        }
    }
    return sfd;
}

int listen_on_port(char *port)
{
    int sfd;
    struct addrinfo *res;

    res = prepare_addrinfo(NULL, port);
    sfd = bind_socket(res);
    freeaddrinfo(res);
    if (sfd != -1) {
        if (listen(sfd, ACCEPT_BUFQ) == -1) {
            fprintf(stderr, "ERROR: listen to port %s failed: ", port);
            close(sfd);
            sfd = -1;
        }
    }
    return sfd;
}

int accept_connection(int sfd)
{
    int rfd;
    socklen_t addr_size;
    struct sockaddr_storage remote_addr;

    addr_size = sizeof(struct sockaddr_storage);
    rfd = accept(sfd, (struct sockaddr*)&remote_addr, &addr_size);
    return rfd;
}

int main(int argc, char *argv[])
{
    int sfd, rfd, received, sent;
    char buf[2048];

    sfd = listen_on_port(argv[1]);
    if (sfd != -1) {
        rfd = accept_connection(sfd);
        if (rfd != -1) {
            received = recv(rfd, buf, sizeof(buf), 0);
            printf("recieved %d bytes\n", received);
            sent = send(rfd, buf, received, 0);
            printf("sent %d bytes\n", sent);
            close(rfd);
            close(sfd);
        }
    }


    return 0;
}
