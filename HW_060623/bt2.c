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

    int num_forks = 10;
    char msg[512] = "HTTP/1.1 200 OK\r\n"
                          "Content-Type: text/html\r\n"
                          "Content-Length: 1234\r\n"
                          "Connection: close\r\n"
                          "\r\n"
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
    for (int i=0;i<num_forks;i++)
    {
        int accept_1 = accept(listener, NULL, NULL);
        if (fork() == 0)
        {
            int socket_1 = accept_1;
            char buf[256];
            while (1)
            {
                int recv_1 = recv(socket_1, buf, sizeof(buf), 0);
                buf[recv_1-1] = 0;
                send(socket_1, msg, strlen(msg), 0);
            }
            
        }
    }

    

    return 0;
}

