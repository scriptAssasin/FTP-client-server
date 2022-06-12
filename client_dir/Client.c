// EVANGELOS FAKORELLIS, sdi1900203

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdbool.h>

#define SIZE 500
#define SA struct sockaddr

// this type of struct is used for data sending between server and client. Gives information about the file if it is directory or file, the relative path and the content
typedef struct transferInfo
{
    bool isDirectory;
    char path[512];
    char content[512];
} transferInfo;

typedef transferInfo *TransferInfo;

void createFile(char *, char *);

int main(int argc, char *argv[])
{
    int sockfd, portno, no_of_bytes;

    struct sockaddr_in serveraddr;
    struct hostent *server;
    char buffer[SIZE], temp[SIZE];

    if (argc < 4)
    {
        printf("usage %s hostname port\n", argv[0]);
        exit(0);
    }

    portno = atoi(argv[2]);

    struct sockaddr_in servaddr, cli;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port = htons(portno);

    // connect the client socket to server socket
    if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");

    bzero(buffer, SIZE);
    strcpy(buffer, argv[3]);
    buffer[strcspn(buffer, "\n")] = 0;

    if (0 != mkdir(buffer, 0777))
        perror("mkdir");

    no_of_bytes = write(sockfd, buffer, strlen(buffer));
    if (no_of_bytes < 0)
    {
        perror("Error in writing");
        return 1;
    }

    bzero(temp, SIZE);
    strcpy(temp, buffer);

    bzero(buffer, SIZE);

    TransferInfo tinfo = malloc(sizeof(*tinfo));

    for (;;)
    {

        no_of_bytes = read(sockfd, tinfo, sizeof(*tinfo));
        if (no_of_bytes < 0 || no_of_bytes == 0)
        {
            perror("Error in reading");
            exit(1);
        }

        if (strcmp(buffer, "No such file in Server Directory\n") != 0)
        {

            if (tinfo->isDirectory == 0)
                createFile(tinfo->content, tinfo->path);
            else
            {
                if (0 != mkdir(tinfo->path, 0777))
                    perror("mkdir");
            }
            bzero(buffer, SIZE);
            printf("File created in Client directory.\n");
        }
        else
        {
            bzero(buffer, SIZE);
            printf("No such file in Server Directory.\n");
        }
    }
    close(sockfd);

    return 0;

}

void createFile(char *array, char *temp)
{
    FILE *fp;
    char ch;
    int i = 0;
    fp = fopen(temp, "w");
    if (fp == NULL)
    {

        char str[512];
        strcpy(str, temp);
        const char *folder = strrchr(str, '/');

        if (0 != mkdir(folder, 0777))
            return;
    }

    fp = fopen(temp, "w");
    while (1)
    {
        ch = array[i++];
        if (ch == '\0')
            break;
        fputc(ch, fp);
    }
    fclose(fp);
}
