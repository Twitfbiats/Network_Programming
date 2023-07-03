#define _GNU_SOURCE

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
#include <dirent.h>
#include <sys/stat.h>

static int port;
static char port_string[5];
const char* header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 1000\r\n\r\n";
static char response[2048];

void dirContent(DIR *d, struct dirent *dir, char path[]);

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

    struct pollfd pollfds[64];
    int client[64];
    int index = 1;
    pollfds[0].fd = listener;
    pollfds[0].events = POLLIN;
    char buf[1024];
    char method[256], path[256];

    while (1)
    {
        int poll_1 = poll(pollfds, 64, -1);

        if (pollfds[0].revents & POLLIN) // new client event
        {
            client[index] = pollfds[index].fd = accept(listener, NULL, NULL);
            pollfds[index++].events = POLLIN;
            printf("New Client Connected !!!\n");
        }

        for (int i=1;i<index;i++)
        {
            if (pollfds[i].revents & (POLLIN | POLLERR))
            {
                int recv_1 = recv(client[i], buf, sizeof(buf), 0);
                if (recv_1 > 0)
                {
                    buf[recv_1] = 0;
                    sscanf(buf, "%s %s", method, path);
                    if (strcmp(path, "/") == 0)
                    {
                        DIR *d;
                        struct dirent *dir;
                        d = opendir(".");
                        dirContent(d, dir, "");
                        
                        printf("%s - %ld\n", response, strlen(response));
                        send(client[i], response, strlen(response), 0);
                    }
                    else if (strstr(path, ".txt") != NULL || strstr(path, ".c") != NULL || strstr(path, ".cpp") != NULL)
                    {
                        char real_path[100] = ".";
                        char buf1[5000000];
                        char content_length[100];
                        strcat(real_path, path);
                        FILE *f1 = fopen(real_path, "rb");
                        struct stat stat1; stat(real_path, &stat1);
                        send(client[i], "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Disposition: inline\r\nContent-Length: ", 88, 0);
                        sprintf(content_length, "%ld", (long)stat1.st_size);
                        printf("Path: %s - Length: %s\n", path, content_length);
                        send(client[i], content_length, strlen(content_length), 0);
                        send(client[i], "\r\n\r\n", 4, 0);
                        while(!feof(f1))
                        {
                            int fread_1 = fread(buf1, sizeof(char), sizeof(buf1), f1);
                            buf1[fread_1] = 0;
                            send(client[i], buf1, fread_1, 0);
                        }
                        fclose(f1);
                    }
                    else if (strstr(path, ".mp3") != NULL)
                    {
                        char real_path[100] = ".";
                        char buf1[5000000];
                        char content_length[100];
                        strcat(real_path, path);
                        FILE *f1 = fopen(real_path, "rb");
                        struct stat stat1; stat(real_path, &stat1);
                        send(client[i], "HTTP/1.1 200 OK\r\nContent-Type: audio/mp3\r\nContent-Disposition: inline\r\nContent-Length: ", 87, 0);
                        sprintf(content_length, "%ld", (long)stat1.st_size);
                        printf("Path: %s - Length: %s\n", path, content_length);
                        send(client[i], content_length, strlen(content_length), 0);
                        send(client[i], "\r\n\r\n", 4, 0);
                        while(!feof(f1))
                        {
                            int fread_1 = fread(buf1, sizeof(char), sizeof(buf1), f1);
                            buf1[fread_1] = 0;
                            send(client[i], buf1, fread_1, 0);
                        }
                        fclose(f1);
                    }
                    else if (strstr(path, ".jpg") != NULL || strstr(path, ".png"))
                    {
                        char real_path[100] = ".";
                        char buf1[5000000];
                        char content_length[100];
                        strcat(real_path, path);
                        FILE *f1 = fopen(real_path, "rb");
                        struct stat stat1; stat(real_path, &stat1);
                        send(client[i], "HTTP/1.1 200 OK\r\nContent-Type: image/jpeg\r\nContent-Disposition: inline\r\nContent-Length: ", 88, 0);
                        sprintf(content_length, "%ld", (long)stat1.st_size);
                        printf("Path: %s - Length: %s\n", path, content_length);
                        send(client[i], content_length, strlen(content_length), 0);
                        send(client[i], "\r\n\r\n", 4, 0);
                        while(!feof(f1))
                        {
                            int fread_1 = fread(buf1, sizeof(char), sizeof(buf1), f1);
                            buf1[fread_1] = 0;
                            send(client[i], buf1, fread_1, 0);
                        }
                        fclose(f1);
                    }
                    else if (strstr(path, ".") == NULL)
                    {
                        DIR *d;
                        struct dirent *dir;
                        char real_path[strlen(path) + 1];
                        char real_path1[strlen(path)];
                        strcpy(real_path, ".");strcat(real_path, path);
                        strcpy(real_path1, path+1);strcat(real_path1, "/");
                        d = opendir(real_path);
                        dirContent(d, dir, real_path1);
                        
                        printf("%s - %ld\n", response, strlen(response));
                        send(client[i], response, strlen(response), 0);
                    }
                }
            }
        }
    }

    close(listener);

    return 0;
}

void dirContent(DIR *d, struct dirent *dir, char path[])
{
    memset(response, 0, sizeof(response));
    strcat(response, header);
    strcat(response, "<!DOCTYPE html><html><body>");
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if (dir->d_type == DT_REG)
            {
                strcat(response, "<a href=\"");
                strcat(response, "http://localhost:");
                sprintf(port_string, "%d", port);
                strcat(response, port_string);
                strcat(response, "/");
                strcat(response, path);
                strcat(response, dir->d_name);
                strcat(response, "\"><i>");
                strcat(response, dir->d_name);
                strcat(response, "</i></a>");
                strcat(response, "<br>");
            }
            else
            {
                strcat(response, "<a href=\"");
                strcat(response, "http://localhost:");
                sprintf(port_string, "%d", port);
                strcat(response, port_string);
                strcat(response, "/");
                strcat(response, path);
                strcat(response, dir->d_name);
                strcat(response, "\">");
                strcat(response, dir->d_name);
                strcat(response, "</a>");
                strcat(response, "<br>");
            }
        }
    }
    strcat(response, "</body></html>");
}
