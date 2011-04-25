
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

int sock=-1;
char data[]="Test message.";
int datalen=sizeof(data);

void do_send()
{
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock==-1) {
        perror("socket");
        goto done;
    }

    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(int));

    struct addrinfo *ai=0;
    struct addrinfo hints;
    int ret;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    if ((ret = getaddrinfo("255.255.255.255", "7570", &hints, &ai))) {
        perror("getaddrinfo");
        goto done;
    }

    ret = sendto(sock, data, datalen, 0,
                 ai->ai_addr, ai->ai_addrlen);
    printf("Sent message.\n");

  done:
    if (sock!=-1)
        close(sock);
    if (ai)
        freeaddrinfo(ai);
}

void do_recv()
{
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock==-1) {
        perror("socket");
        goto done;
    }

    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(int));

    struct addrinfo *ai=0;
    struct addrinfo hints;
    int ret;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    if ((ret = getaddrinfo("255.255.255.255", "7570", &hints, &ai))) {
        perror("getaddrinfo");
        goto done;
    }

    if (bind(sock, ai->ai_addr, ai->ai_addrlen) < 0) {
        perror("bind");
        goto done;
    }

    char d[256];
    if (recvfrom(sock, d, datalen, 0, ai->ai_addr, &ai->ai_addrlen) < 0) {
        perror("recvfrom");
        goto done;
    }

    printf("Message received: `%s'\n", d);

  done:
    if (sock!=-1)
        close(sock);
    if (ai)
        freeaddrinfo(ai);
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("test <send/recv>\n");
        return 0;
    }
    if (strcmp(argv[1], "send")==0) {
        printf("Testing broadcast sending.\n");
        do_send();
    }
    else if (strcmp(argv[1], "recv")==0) {
        printf("Testing broadcast receiving.\n");
        do_recv();
    }
    else {
        printf("Unknown action.\n");
        return 1;
    }
    return 0;
}
