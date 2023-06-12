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
static int num_thread = 8;
static char msg[512] = "HTTP/1.1 200 OK\r\n"
"Content-Type: text/html\r\n"
"Content-Length: 1234\r\n"
"Connection: close\r\n\r\n"
"<!DOCTYPE html>\r\n"
"<html>\r\n"
"<head>\r\n"
"  <title>Welcome to My Webpage</title>\r\n"
"</head>\r\n"
"<body>\r\n"
"  <h1>Hello, world!</h1>\r\n"
"  <p>This is the content of my webpage.</p>\r\n"
"</body>\r\n"
"</html>";

void *exec(void *args);

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

    for (int i=0;i<num_thread;i++)
    {
        pthread_t pthread_t_temp;

        if (pthread_create(&pthread_t_temp, NULL, exec, &listener))
        {
            printf("Thread init error\n");
            sched_yield();
        }
        pthread_detach(pthread_t_temp);

        pthread_join(pthread_t_temp, NULL);
    }

    while (1)
    {

    }

    return 0;
}

void *exec(void *args)
{
    int listener = *(int *)args;
    int accept_1 = accept(listener, NULL, NULL);

    send(accept_1, msg, strlen(msg), 0);
}