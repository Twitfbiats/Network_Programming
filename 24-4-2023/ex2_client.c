#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>

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
    FILE *text = fopen("text","rb");

    while (!feof(text))
    {
        int read_1 = fread(buf, 1, sizeof(buf), text);
        int send_1 = send(client, buf, read_1, 0);
    }

    // Ket thuc, dong socket
    fclose(text);
    close(client);

    return 0;
}