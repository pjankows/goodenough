/* author: pjankows@gmail.com */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

struct addrinfo* prepare_addrinfo(char *host, char *port)
{
    int status;
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    status = getaddrinfo(host, port, &hints, &res);
    if (status != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        res = NULL;
    }
    return res;
}

int get_socket(struct addrinfo *res)
{
    int s = -1;
    if (res != NULL)
    {
        s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    }
    return s;
}

int main(int argc, char *argv[])
{
    int s;
    struct addrinfo *res;
    res = prepare_addrinfo(argv[1], argv[2]);
    if (res != NULL) {
        printf("%p\n", res);
        s = get_socket(res);
        printf("%d\n", s);
    }
    else {
        printf("NULL\n");
        return 1;
    }
    freeaddrinfo(res);
    return 0;
}
