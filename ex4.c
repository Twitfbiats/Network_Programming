#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> // close()

static int port;
static char logfile_name[256];

char* getTime()
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char *time = malloc(100);
    sprintf(time, "now: %d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    return time;
}

// kiem tra tham so dau vao co dang: port ten_file ten_file
// VD: ./ex2 8080 hello.dat client.dat
int checkParam(int argc, char const *argv[])
{
    if (argc != 3)
    {
        printf("Check params\n");
        return 1;
    }

    // port check
    if (sscanf(
        argv[1],
        "%d",
        &port
    ) != 1)
    {
        printf("Check port\n");
        return 1;
    }

    // file_name check
    if (sscanf(
        argv[2],
        "%s",
        logfile_name
    ) != 1)
    {
        printf("Check log file\n");
        return 1;
    }
}

int main(int argc, char const *argv[])
{
    // kiem tra tham so dau vao
    checkParam(argc, argv);

    // tao socket
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1) // neu co loi se luu trong errno
    {
        printf("Unable to init socket: %d - %s\n",
            errno, 
            strerror(errno)
        );
        return 1;
    }
    printf("Success initializing socket\n");

    // tao thong tin cho server
    struct sockaddr_in sck_addr;
    sck_addr.sin_family = AF_INET;
    sck_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    sck_addr.sin_port = htons(port);

    // gan thong tin server voi socket
    // neu bind = 1 --> loi xay ra
    if (bind(
        listener,
        (struct sockaddr *)&sck_addr,
        sizeof(sck_addr)
        ))
    {
        printf("Unable to bind socket: %d - %s\n",
            errno, 
            strerror(errno)
        );

        return 1;
    }
    printf("Success binding at %s:%d and listener = %d\n",
        (inet_ntoa(
            ((struct sockaddr_in*)&sck_addr)->sin_addr // chuyen tu kieu dia chi ve string
        )), 
        ntohs(sck_addr.sin_port), // network order to host order - 16 bit
        listener
    );

    // khoi tao 5 ket noi cho client
    // listen = 1 --> loi
    if (listen(listener, 5))
    {
        printf("Unable to listen: %d - %s\n",
            errno, 
            strerror(errno)
        );

        return 1;
    }
    printf("Success listening and listener = %d\n", listener);

    // thong tin client se duoc luu o day
    struct sockaddr_in sck_addr_client;
    int sck_addr_client_len = sizeof(sck_addr_client);
    // chap nhan ket noi tu client
    int accept_1 = accept(
                    listener,
                    (struct sockaddr*)&sck_addr_client,
                    &sck_addr_client_len
                    );

    if (accept_1 == -1)
    {
        printf("Unable to accept: %d - %s\n",
            errno, 
            strerror(errno)
        );

        return 1;
    }

    // cai nay ko can quan tam
    printf("Success accepting client %s:%d and listener = %d\n",
        inet_ntoa(sck_addr_client.sin_addr),
        sck_addr_client.sin_port,
        listener
    );

    FILE *client_file = fopen(logfile_name, "wb"); // mo file de ghi - kieu byte
    char buf[2048];

    char *time,port[6];
    char client_info[24] = "\n"; // max 255.255.255.255:99999
    sprintf(port, "%d", sck_addr_client.sin_port);
    strcat(client_info, inet_ntoa(sck_addr_client.sin_addr));strcat(client_info, ":");strcat(client_info, port);

    // nhan data tu client va ghi vao file
    while (1)
    {
        int recv_1 = recv(accept_1, buf, sizeof(buf), 0);
        time = getTime();
        if (recv_1 == 1) break;
        buf[recv_1] = 0;
        fwrite(client_info, 1, strlen(client_info), client_file); 
        fwrite(" at ", 1, 4, client_file); 
        fwrite(time, 1, strlen(time), client_file); 
        fwrite(": ", 1, 2, client_file); 
        fwrite(buf, 1, strlen(buf), client_file); 
    }
    
    fclose(client_file);

    close(listener);close(accept_1);
    return 0;
}

//chay server: ./ex2 8081 hello.dat client.dat
// hello.dat va client.dat dat o cung folder voi ex2
// chay client: nc localhost 8081
