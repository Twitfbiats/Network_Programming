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

    int client[5];
    int validate[5];memset(validate, 0, sizeof(validate));
    char *id[5];
    char message[1024];

    int index = 0;
    char buf[256];
    int receive_1, scan_1, accept_1;
    fd_set fdread;
    char check1[100], check2[100], check3[100];

    struct sockaddr client_1;
    int client1_size = sizeof(client_1);
    while (1)
    {
        FD_ZERO(&fdread);
        FD_SET(listener, &fdread);
        for (int i=0;i<index;i++)
        {
            FD_SET(client[i], &fdread);
        }

        int select_1 = select(100, &fdread, NULL, NULL, NULL);

        if (FD_ISSET(listener , &fdread))
        {
            int accept_1 = accept(listener, &client_1, &client1_size);
            client[index++] = accept_1;
            send(accept_1, "client_id : client_name\n", 24, 0);
        }

        for (int i=0;i<index;i++)
        {
            if (FD_ISSET(client[i], &fdread))
            {
                receive_1 = recv(client[i], buf, sizeof(buf), 0);
                buf[receive_1] = 0;
                if (validate[i] == 0)
                {
                    if (sscanf(buf, "%s : %s %s", check1, check2, check3) != 2)
                    {
                        send(client[i], "client_id : client_name\n", 24, 0);
                    }
                    else
                    {
                        validate[i] = 1;
                        id[i] = malloc(strlen(check1));
                        strcpy(id[i], check1);
                        printf("new id: %s\n", id[i]);
                    }
                }
                else
                {
                    for (int j=0;j<index;j++)
                    {
                        sprintf(message, "%s : %s", id[i], buf);
                        send(client[j], message, strlen(message), 0);
                    }
                }
            }
        }
    }

    close(listener);
    for (int i=0;i<sizeof(client);i++) close(client[i]);

    return 0;
}
