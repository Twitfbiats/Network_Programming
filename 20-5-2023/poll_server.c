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

    int validate[64];memset(validate, 0, sizeof(validate)); // validation
    int validate_temp[64];memset(validate_temp, 0, sizeof(validate_temp)); // temp is used for copy
    char *id[64]; char *id_temp[64]; // client id
    char message[1024]; 

    int index = 1, index1 = 0;
    char buf[256];
    int receive_1, scan_1, accept_1;
    struct pollfd pollfds[64]; // 63 client and 1 for listener
    struct pollfd pollfds_temp[64];
    char check1[100], check2[100], check3[100];

    struct sockaddr client_1;
    int client1_size = sizeof(client_1);
    pollfds[0].fd = listener;
    pollfds[0].events = POLLIN;
    while (1)
    {
        int poll_1 = poll(pollfds, 64, -1);

        if (pollfds[0].revents & POLLIN) // new client event
        {
            int accept_1 = accept(listener, &client_1, &client1_size);
            pollfds[index].fd = accept_1;
            pollfds[index++].events = POLLIN;
            send(accept_1, "client_id : client_name\n", 24, 0);
        }

        for (int i=1;i<index;i++)
        {
            if (pollfds[i].revents & (POLLIN | POLLERR)) // message event
            {
                receive_1 = recv(pollfds[i].fd, buf, sizeof(buf), 0);
                buf[receive_1] = 0;
                if (receive_1 == 0) // when someone disconnect
                {
                    // reset event array after deletion using temp
                    int pollfd_delete = pollfds[i].fd;
                    for (int k=0;k<index;k++)
                    {
                        if ((pollfds[k].fd != pollfd_delete) && (pollfds[k].fd != 0))
                        {
                            pollfds_temp[index1++] = pollfds[k];
                        }
                    }
                    memset(pollfds, 0, sizeof(pollfds));
                    memcpy(pollfds, pollfds_temp, sizeof(pollfds_temp));
                    memset(pollfds_temp, 0, sizeof(pollfds_temp)); index1 = 1;

                    // reset validation array and client id array after deletion
                    for (int k=1;k<index;k++)
                    {
                        if (k != i)
                        {
                            validate_temp[index1] = validate[k];

                            id_temp[index1] = malloc(sizeof(id[k]));
                            memcpy(id_temp[index1++], id[k], sizeof(id[k]));
                        }
                    }
                    memset(validate, 0, sizeof(validate));
                    memcpy(validate, validate_temp, sizeof(validate_temp));
                    memset(validate_temp, 0, sizeof(validate_temp));

                    memset(id, 0, sizeof(id));
                    memcpy(id, id_temp, sizeof(id_temp));
                    memset(id_temp, 0, sizeof(id_temp)); index1 = 0;

                    index--;
                }
                else
                {
                    if (validate[i] == 0) // if client isn't valid
                    {
                        if (sscanf(buf, "%s : %s %s", check1, check2, check3) != 2)
                        {
                            send(pollfds[i].fd, "client_id : client_name\n", 24, 0);
                        }
                        else
                        {
                            validate[i] = 1;
                            id[i] = malloc(strlen(check1));
                            strcpy(id[i], check1);
                            printf("new id: %s\n", id[i]);
                        }
                    }
                    else // if client is valid
                    {
                        for (int j=1;j<index;j++)
                        {
                            sprintf(message, "%s : %s", id[i], buf);
                            send(pollfds[j].fd, message, strlen(message), 0);
                        }
                    }
                }
            }
        }
    }

    close(listener);

    return 0;
}
