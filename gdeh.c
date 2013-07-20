/* author: pjankows@gmail.com */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

struct addrinfo* prepare_addrinfo(char *host)
{
    int status;
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    status = getaddrinfo(host, NULL, &hints, &res);
    if (status != 0)
    {
        res = NULL;
    }
    return res;
}

int main(int argc, char *argv[])
{
    struct addrinfo *res;
    res = prepare_addrinfo(argv[1]);
    if (res != NULL) {
        printf("%p\n", res);
    }
    else {
        printf("NULL\n");
    }
    freeaddrinfo(res);
    return 0;
}
