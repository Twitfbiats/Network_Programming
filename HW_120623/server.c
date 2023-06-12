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

static int port;
static int num_client = 3;

typedef struct
{
    int* all_client;
    int client;
} data_struct;


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

    if (listen(listener, num_client)) 
    {
        perror("listen() failed");
        return 1;
    }

    int client[num_client];memset(client, 0, sizeof(client));
    data_struct data_struct1;
    data_struct1.all_client = client;
    pthread_t pthread_t1;
    while (1)
    {
        int accept_1 = accept(listener, NULL, NULL);

        pthread_t pthread_t_temp = pthread_t1;
        data_struct1.client = accept_1;
        for (int i=0;i<num_client;i++)
        {
            if (data_struct1.all_client[i] == 0)
            {
                if (pthread_create(&pthread_t_temp, NULL, exec, (void *)&data_struct1))
                {
                    printf("Thread init error\n");
                    return 0;
                }
                pthread_detach(pthread_t_temp);
                break;
            }
        }
    }
    return 0;
}

void *exec(void *args)
{
    data_struct data_struct1 = *(data_struct *)args;
    int client = data_struct1.client;
    printf("%d\n", client);
    for (int i=0;i<num_client;i++)
    {
        if (data_struct1.all_client[i] == 0)
        {
            data_struct1.all_client[i] = client;
            break;
        }
    }

    char buf[256], msg[514];
    char client_name[256];
    while (1)
    {
        int recv_1 = recv(client, buf, sizeof(buf), 0);
        if (recv_1 == 0)
        {
            for (int i=0;i<num_client;i++)
            {
                if (data_struct1.all_client[i] == client) data_struct1.all_client[i] = 0;
            }
            pthread_cancel(pthread_self());
        }
        else if (recv_1 > 0)
        {
            buf[recv_1-1] = 0;
            if (sscanf(buf ,"client_id: %s", client_name) != 1)
            {
                send(client, "Input incorrect, Type again in this format: client_id: your_name\n", 65, 0);
            }
            else break;
        }
    }
    
    while (1)
    {
        int recv_1 = recv(client, buf, sizeof(buf), 0);
        if (recv_1 == 0)
        {
            for (int i=0;i<num_client;i++)
            {
                if (data_struct1.all_client[i] == client) data_struct1.all_client[i] = 0;
            }
            pthread_cancel(pthread_self());
        }
        else if (recv_1 > 0)
        {
            buf[recv_1] = 0;
            sprintf(msg, "%s: %s", client_name, buf);
            for (int i=0;i<num_client;i++)
            {
                send(data_struct1.all_client[i], msg, strlen(msg), 0);
            }
        }
    }
}
