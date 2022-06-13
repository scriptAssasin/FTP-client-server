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
#include <arpa/inet.h>
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

// create file function, creates the given file with the given content
void createFile(char *, char *);

int main(int argc, char *argv[])
{
    int sockfd, portno, no_of_bytes;

    struct sockaddr_in serveraddr;
    struct hostent *server;
    char buffer[SIZE], temp[SIZE];

    if (argc < 7)
    {
        printf("usage ./remoteClient -i <server_ip> -p <server_port> -d <directory> \n");
        exit(0);
    }

    portno = atoi(argv[4]);

    struct sockaddr_in servaddr, cli;

    // socket create
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
    servaddr.sin_addr.s_addr = inet_addr(argv[2]);
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
    strcpy(buffer, argv[6]);
    // remove newline from the end of buffer
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

    // allocate memory for a TransferInfo object
    TransferInfo tinfo = malloc(sizeof(*tinfo));

    for (;;)
    {

        no_of_bytes = read(sockfd, tinfo, sizeof(*tinfo));
        if (no_of_bytes < 0 || no_of_bytes == 0)
        {
            perror("Error in reading");
            exit(1);
        }
        printf("tinfo dirbool: %d\n", tinfo->isDirectory);

        // if (tinfo->isDirectory == 0)
            createFile(tinfo->content, tinfo->path);
        // else
        // {
        //     if (0 != mkdir(tinfo->path, 0777))
        //         perror("mkdir");
        // }
        bzero(buffer, SIZE);
        printf("File created in Client directory.\n");
    }
    close(sockfd);

    return 0;
}

void createFile(char *array, char *path)
{
    FILE *fp;
    char ch;
    int i = 0;
    fp = fopen(path, "w");
    if (fp == NULL)
    {

        char str[512];
        strcpy(str, path);
        const char *folder = strrchr(str, '/');

        if (0 != mkdir(folder, 0777))
            return;
    }

    fp = fopen(path, "w");
    while (1)
    {
        ch = array[i++];
        if (ch == '\0')
            break;
        fputc(ch, fp);
    }
    fclose(fp);
}
