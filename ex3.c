#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

static int byte1,byte2,byte3,byte4;
static int port;

int check_IP_and_Port(int argc, char const *argv[])
{
    // neu so tham so khac 3 thi thoat 
    // - argv[0] la ten chuong trinh
    // - argv[1] la IP address
    // - argv[2] la port
    if (argc != 3)
    {
        printf("Check params");
        return 1;
    }

    // kiem tra xem ip co dung dinh dang x.x.x.x
    if (sscanf(
        argv[1],
        "%d.%d.%d.%d",
        &byte1,&byte2,&byte3,&byte4
        ) != 4
    )
    {
        printf("Check params IP");
        return 1;
    }

    // kiem tra xem port co phai mot so
    if (sscanf(argv[2], "%d", &port) != 1)
    {
        printf("Check params port");
        return 1;
    }
}

int main(int argc, char const *argv[])
{
    // kiem tra tham so dong lenh 
    // voi dia chi ip va port
    check_IP_and_Port(argc, argv);

    // khoi tao socket ket noi den server
    // su dung ipv4 va TCP
    int socket_1 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // them thong tin cho server
    struct sockaddr_in sckaddr_server;
    char address[16];
    sprintf(address, "%d.%d.%d.%d",byte1,byte2,byte3,byte4);
    sckaddr_server.sin_family = AF_INET; // ipv4
    sckaddr_server.sin_addr.s_addr = inet_addr(address); // gan dia chi ip cho server
    sckaddr_server.sin_port = htons(port); // gan port cho server

    // ket noi den server 
    int connect_1 = connect(socket_1, (struct sockaddr*)&sckaddr_server, sizeof(sckaddr_server));

    // loi~ se duoc luu trong errno, strerror(errno) de in ra ten loi~
    if (connect_1 == -1)
    {
        printf("Unable to connect: %d - %s\n",
            errno,
            strerror(errno)
        );
        return 1;
    } printf("Connect success\n");

    char buf[1000];
    char buf1[1000];
    strcpy(buf,"\n"); //ko nen de buf rong~
    printf("MSSV: ");
//-- thong tin cho vao buf1         |reset ban phim| copy buf1->buf  | reset buf1
    fgets(buf1, sizeof(buf1), stdin);fflush(stdin);  strcat(buf,buf1); memset(buf1, 0, sizeof(buf1));
    printf("Full Name: ");
    fgets(buf1, sizeof(buf1), stdin);fflush(stdin);strcat(buf,buf1);memset(buf1, 0, sizeof(buf1));
    printf("Date Of Birth: ");
    fgets(buf1, sizeof(buf1), stdin);fflush(stdin);strcat(buf,buf1);memset(buf1, 0, sizeof(buf1));
    printf("CPA: ");
    fgets(buf1, sizeof(buf1), stdin);fflush(stdin);strcat(buf,buf1);memset(buf1, 0, sizeof(buf1));
    printf("\n%s",buf);

    char *p = malloc(strlen(buf)); //cap phat vung nho moi cho p
    strcpy(p,buf);
    printf("%d",(int)strlen(p));

    send(socket_1, p, strlen(p), 0); // gui cho server

    return 0;
}

// vao linux nhap: nc -4lv 127.0.0.1 50000 (l la listen mode danh cho server)
// dich file
// chay file nay: ./ten_file 127.0.0.1 50000
