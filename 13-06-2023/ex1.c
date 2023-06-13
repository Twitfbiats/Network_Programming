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
#include <pthread.h>
#include <sys/stat.h>

static int port;
static char *files[] = {"test1.txt", "test2.txt"};
static char total_files = 2;

void *exec(void *);

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

        pthread_t pthread_t_temp;
        if (pthread_create(&pthread_t_temp, NULL, exec, &accept_1))
        {
            printf("Thread init error\n");
            return 0;
        }
        pthread_detach(pthread_t_temp);
    }

    return 0;
}

void *exec(void *args)
{
    int socket_1 = *(int *)args;

    char buf[256];
    char data[1024];

    if (files[0] == NULL)
    {
        send(socket_1, "ERROR No files to download \r\n", 29, 0);
        pthread_cancel(pthread_self());
    }
    else
    {
        sprintf(data, "OK %d\r\n", total_files);
        for (int i=0;i<total_files;i++)
        {
            strcat(data, files[i]);
            strcat(data, "\r\n");
        }
        strcat(data, "\r\n\r\n");
        send(socket_1, data, strlen(data), 0);

        while (1)
        {
            int recv_1 = recv(socket_1, buf, sizeof(buf), 0);
            if (recv_1 > 0)
            {
                buf[recv_1 - 1] = 0;
                for (int i=0;i<total_files;i++)
                {
                    if (strcmp(files[i], buf) == 0)
                    {
                        char data1[2048]; memset(data1, 0, sizeof(data1));
                        struct stat stat_1;
                        stat(buf, &stat_1);
                        int file_size = stat_1.st_size;
                        FILE *f1 = fopen(buf, "rb");
                        char buf1[1024]; memset(buf1, 0, sizeof(buf1));
                        char temp[100];

                        sprintf(data1, "OK %d\r\n", file_size);
                        while(fgets(temp, 100, f1)) strcat(buf1, temp);
                        strcat(data1, buf1);

                        send(socket_1, data1, strlen(data1), 0);
                        
                        goto found;
                    }
                }
                send(socket_1, "FILE NOT FOUND, SEND AGAIN!!!\n", 30, 0);
            }
        }
        
        found: pthread_cancel(pthread_self());
    }
}