#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>

static int port;

int checkParam(int argc, char const *argv[])
{
    if (argc != 2)
    {
        printf("Check params\n");
        return 1;
    }

    // port check
    if (sscanf(
        argv[1],
        "%d",
        &port
    ) != 1)
    {
        printf("Check port\n");
        return 1;
    }
}

int main(int argc, char const *argv[])
{
    checkParam(argc, argv);

    int socket_receiver = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    struct sockaddr_in receiver_info;
    receiver_info.sin_family = AF_INET;
    receiver_info.sin_addr.s_addr = htonl(INADDR_ANY);
    receiver_info.sin_port = htons(port);

    int bind_1 = bind(socket_receiver, (struct sockaddr*)&receiver_info, sizeof(receiver_info));
    if (bind_1)
    {
        printf("Unable to bind: %d - %s\n",
            errno,
            strerror(errno)
        );
        return 1;
    } printf("Bind success\n");

    char buf[256];
    FILE *f = fopen("receive.dat", "wb");
    while (1)
    {
        int recv_1 = recvfrom(socket_receiver, buf, sizeof(buf), 0, NULL, NULL);
        printf("Receive: %d\n",recv_1);
        buf[recv_1] = 0;
        fwrite(buf, 1, strlen(buf), f);

        if (strstr(buf,"this is the end")) break;
    }
    
    fclose(f);

    return 0;
}
