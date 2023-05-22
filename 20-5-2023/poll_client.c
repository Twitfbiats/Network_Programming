#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <poll.h>

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

int main() {
    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(9000); 

    if (connect(client, (struct sockaddr *)&addr, sizeof(addr))) {
        perror("connect() failed");
        return 1;
    }
    
    char buf[256];
    struct pollfd pollfds[2];
    pollfds[0].fd = STDIN_FILENO;
    pollfds[0].events = POLLIN;
    pollfds[1].fd = client;
    pollfds[1].events = POLLIN;

    int select_1, recv_1;

    while (1)
    {
        int poll_1 = poll(pollfds, 2, -1);

        if (pollfds[0].revents & POLLIN)
        {
            fgets(buf, sizeof(buf), stdin);
            send(client, buf, strlen(buf), 0);
        }

        if (pollfds[1].revents & POLLIN)
        {
            recv_1 = recv(client, buf, sizeof(buf), 0);
            if (recv_1 <= 0)
                break;
            buf[recv_1] = 0;
            printf("%s%s%s\n", MAG, buf, RESET);
        }
    }

    // Ket thuc, dong socket
    close(client);

    return 0;
}