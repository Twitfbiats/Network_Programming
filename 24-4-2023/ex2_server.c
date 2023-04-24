#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

int count_string(char *source, char *find)
{
    int count = 0;
    char *tmp = source;
    while ((tmp = strstr(tmp, find)) != NULL)
    {
        tmp += strlen(find);
        count++;
    }
    return count;
}

int main() 
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
    addr.sin_port = htons(9000);

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

    int client = accept(listener, NULL, NULL);
    if (client == -1)
    {
        perror("accept() failed");
        return 1;
    }
    printf("New client connected: %d\n", client);

    // Truyen nhan du lieu
    char buf[40];
    char firstBuf[20],secondBuf[20];
    memset(firstBuf,'$',sizeof(firstBuf));
    int count = 0;

    while (1)
    {
        int recv_1 = recv(client, secondBuf, sizeof(secondBuf), 0);
        secondBuf[recv_1] = 0;
        strcpy(buf, firstBuf);
        strcat(buf, secondBuf);
        count = count + (count_string(buf, "123456789") - count_string(firstBuf, "123456789"));

        memset(firstBuf, 0,sizeof(firstBuf));
        strcpy(firstBuf, secondBuf);

        if (recv_1 == 0) break;
    }
    
    printf("count: %d\n",count);

    

    close(client);
    close(listener);    

    return 0;
}