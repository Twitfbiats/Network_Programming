#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/select.h>
#include <poll.h>
#include <sys/wait.h>
#include <signal.h>

static int port;

int main(int argc, char const *argv[])
{
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1)
    {
        perror("socket() failed");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    sscanf(argv[1], "%d", &port);
    addr.sin_port = htons(port);

    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr))) 
    {
        perror("bind() failed");
        return 1;
    }

    if (listen(listener, 5)) 
    {
        perror("listen() failed");
        return 1;
    }

    while (1)
    {
        int accept_1 = accept(listener, NULL, NULL);
        if (fork() == 0)
        {
            int socket_1 = accept_1;
            char buf[256];
            char command[266];
            char read_1[1024];
            while (1)
            {
                int recv_1 = recv(socket_1, buf, sizeof(buf), 0);
                buf[recv_1-1] = 0;
                if (recv_1 > 0)
                {
                    FILE *f1 = fopen("out.txt", "rb");
                    sprintf(command, "%s > out.txt", buf);
                    printf("command: %s\n", command);
                    if (system(command) == 0)
                    {
                        int fread_1 = fread(read_1, 1, sizeof(read_1), f1);
                        if (fread_1 != 0) read_1[fread_1] = 0;
                        send(socket_1, read_1, strlen(read_1), 0);
                    }
                    else send(socket_1, "Sai lenh r em\n", 14, 0);
                    
                    fclose(f1);
                }
            }
            
        }
    }

    return 0;
}

