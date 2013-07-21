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
#define REQ_BUF 4096
#define RESP_BUF 4096

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
    int socketfd, b;
    int port;
    char ip[INET6_ADDRSTRLEN];
    socketfd = get_socket(res);
    if (socketfd != -1) {
        b = bind(socketfd, res->ai_addr, res->ai_addrlen);
        port = get_port_and_ip(res, ip);
        if (b != 0) {
            fprintf(stderr, "ERROR: bind to port %d on %s failed: ", port, ip);
            perror(NULL);
            close(socketfd);
            socketfd = -1;
        }
        else {
            printf("bind to port %d on %s OK\n", port, ip);
        }
    }
    return socketfd;
}

int listen_on_port(char *port)
{
    int socketfd;
    struct addrinfo *res;

    res = prepare_addrinfo(NULL, port);
    socketfd = bind_socket(res);
    freeaddrinfo(res);
    if (socketfd != -1) {
        if (listen(socketfd, ACCEPT_BUFQ) == -1) {
            fprintf(stderr, "ERROR: listen to port %s failed: ", port);
            close(socketfd);
            socketfd = -1;
        }
    }
    return socketfd;
}

int accept_connection(int socketfd)
{
    int remotefd;
    socklen_t addr_size;
    struct sockaddr_storage remote_addr;

    addr_size = sizeof(struct sockaddr_storage);
    remotefd = accept(socketfd, (struct sockaddr*)&remote_addr, &addr_size);
    return remotefd;
}

int connect_to(char *host, char *port)
{
    int socketfd;
    struct addrinfo *res;

    res = prepare_addrinfo(host, port);
    socketfd = get_socket(res);
    if (socketfd != -1) {
        if (connect(socketfd, res->ai_addr, res->ai_addrlen) == -1) {
            fprintf(stderr, "ERROR: connect %s:%s failed: ", host, port);
            perror(NULL);
            close(socketfd);
            socketfd = -1;
        }
    }
    freeaddrinfo(res);
    return socketfd;
}

int main(int argc, char *argv[])
{
    int socketfd, remotefd, targetfd;
    int received, sent;
    int target_sent, target_received;
    char req[REQ_BUF], resp[RESP_BUF];

    socketfd = listen_on_port(argv[1]);
    if (socketfd != -1) {
        remotefd = accept_connection(socketfd);
        if (remotefd != -1) {
            received = recv(remotefd, req, sizeof(req), 0);
            printf("received %d bytes\n", received);

            targetfd = connect_to(argv[2], argv[3]);
            if (targetfd != -1) {
                target_sent = send(targetfd, req, received, 0);
                printf("sent %d bytes to target\n", target_sent);
                target_received = recv(targetfd, resp, sizeof(resp), 0);
                printf("got %d bytes from target\n", target_received);
            }
            else {
                perror("ERROR: unable to connect to target");
            }
            sent = send(remotefd, resp, target_received, 0);
            printf("sent %d bytes\n", sent);
            close(remotefd);
            close(socketfd);
        }
    }


    return 0;
}
