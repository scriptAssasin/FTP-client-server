
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdbool.h>
#define SIZE 500
#define thread_pool_size 2
#define queue_size 2

typedef struct toSend
{
    bool isDirectory;
    char path[512];
    char content[4096];
} toSend;

typedef toSend *ToSend;

// void getfile(char *, ToSend *);
void makefile(char *, ToSend);

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, no_of_bytes;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char buffer[SIZE], temp[SIZE];
    char dir_name[512];
    ToSend paralavou;

    if (argc < 4)
    {
        printf("usage %s hostname port\n", argv[0]);
        exit(0);
    }

    portno = atoi(argv[2]);
    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        printf("ERROR, no such host\n");
        exit(0);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("Error in socket");

    bzero((char *)&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serveraddr.sin_addr.s_addr, sizeof(server->h_addr));
    serveraddr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
        error("Error in connecting the socket");

    // while(1)
    // {
    bzero(buffer, SIZE);
    // fgets(buffer, SIZE - 1, stdin);
    strcpy(buffer, argv[3]);
    buffer[strcspn(buffer, "\n")] = 0;

    no_of_bytes = write(sockfd, buffer, SIZE - 1);
    if (no_of_bytes < 0)
    {
        error("Error in writing");
        return 1;
    }
    bzero(temp, SIZE);
    strcpy(temp, buffer);

    printf("Message from Server:\n\n");
    bzero(buffer, SIZE);

    for (int z = 0; z < 5; z++)
    {

    no_of_bytes = read(sockfd, paralavou, SIZE - 1);
    printf("%d\n", paralavou->isDirectory);

    if (paralavou->isDirectory == 0)
    {

        if (no_of_bytes < 0)
        {
            error("Error in reading");
            // break;
        }

        if (strcmp(buffer, "No such file in Server Directory\n") != 0)
        {
            makefile(buffer, paralavou);
            bzero(buffer, SIZE);
            printf("File created in Client directory.\n");
        }
        else
        {
            bzero(buffer, SIZE);
            printf("No such file in Server Directory.\n");
        }
    }
    else
    {
         mkdir(paralavou->path, 0700);
    }

    }
    close(sockfd);

    return 0;

    // if put has been sent then read file contents in buffer and again write them to server

    // }
}

// void getfile(char *array, ToSend *temp)
// {
//     FILE *fp;
//     char ch;
//     int i = 0;
//     fp = fopen(temp, "r");
//     bzero(array, SIZE);
//     if (fp == NULL)
//     {
//         strcpy(array, "No file found\n");
//     }
//     else
//     {
//         while (1)
//         {
//             ch = fgetc(fp);
//             if (ch == EOF)
//                 break;
//             array[i++] = ch;
//         }
//         fclose(fp);
//     }
// }

void makefile(char *array, ToSend temp)
{
    // printf("%s\n", temp->content);
    FILE *fp;
    char ch;
    int i = 0;
    fp = fopen(temp->path, "w");
    while (1)
    {
        ch = temp->content[i++];
        if (ch == '\0')
            break;
        fputc(ch, fp);
    }
    fclose(fp);
}
