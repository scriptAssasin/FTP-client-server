#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/stat.h>
#include "ADTQueue.h"
#include <stdbool.h>

#define SA struct sockaddr
#define SIZE 500
#define queuesize 5

typedef struct queueFile
{
    bool isDirectory;
    char path[512];
    int socket_file_descriptor;
} queueFile;

typedef queueFile *QueueFile;

typedef struct toSend
{
    bool isDirectory;
    char path[512];
    int socket_file_descriptor;
    char content[512];
} toSend;

typedef toSend *ToSend;

pthread_t workerthreads[100];
Queue queue;

void getfile(char *, char *, int *connfd);
void listFiles(const char *path);
void makefile(char *, char *, ToSend);

int i = 2;

void error(char *msg)
{
    perror(msg);
    exit(1);
}

void *worker(void *p)
{
    int no_of_bytes;
    char buffer[SIZE];
    bzero(buffer, SIZE);
    for (queueNode node = queue_first(queue); node != queue_EOF; node = queue_next(queue, node))
    {
        QueueFile val = (QueueFile)queue_node_value(queue, node);

        if (val->isDirectory == 0)
        {
            ToSend file = malloc(sizeof(*file));
            file->isDirectory = val->isDirectory;
            strcpy(file->path, val->path);

            makefile(buffer, val->path, file);
            strcpy(file->content, buffer);

            no_of_bytes = write(val->socket_file_descriptor, file, sizeof(*file));

            if (no_of_bytes < 0)
                error("Write");

            bzero(buffer, SIZE);
            bzero(file->content, SIZE);
        }
        else
        {
            ToSend file = malloc(sizeof(*file));
            file->isDirectory = val->isDirectory;
            strcpy(file->path, val->path);


            no_of_bytes = write(val->socket_file_descriptor, file, sizeof(*file));

            if (no_of_bytes < 0)
                error("Write");

            bzero(buffer, SIZE);
            bzero(file->content, SIZE);
        }
        printf("%d - %s \n", val->isDirectory, val->path);
    }

    // printf("---%d---", *(int*)p++);

    pthread_exit(&i);
}

int is_regular_file(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

void listFilesRecursively(char *basePath, int *connfd)
{
    pthread_t self;
    self = pthread_self();

    char path[1000];
    struct dirent *dp;
    DIR *dir = opendir(basePath);

    // Unable to open directory stream
    if (!dir)
        return;

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            QueueFile file = malloc(sizeof(*file));
            file->isDirectory = (bool)is_regular_file(path);
            printf("---%d----", file->isDirectory);
            // Construct new path from our base path
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);
            // printf("%s, %d\n", path, is_regular_file(path));
            if ((bool)is_regular_file(path) == 0)
                printf("[Thread: %ld]: Adding file %s to the queue... \n", self, path);
            strcpy(file->path, path);
            file->socket_file_descriptor = *connfd;

            queue_insert(queue, file);

            listFilesRecursively(path, connfd);
        }
    }

    closedir(dir);
}

void *communicationThreadFunc(void *connfd)
{
    int no_of_bytes;
    char buffer[SIZE], temp[SIZE];
    bzero(buffer, SIZE);

    no_of_bytes = read(*(int *)connfd, buffer, SIZE - 1);

    if (no_of_bytes < 0)
        error("Read");
    else if (no_of_bytes == 0)
    {

        printf("\nClient Disconnected\n");
        // bzero(buffer, SIZE);
    }

    // printf("\nMessage from Client: %s \n", buffer);

    // listFiles("/dummy");
    bzero(temp, SIZE);
    strcpy(temp, buffer);
    getfile(buffer, temp, (int *)connfd);
    for (int z = 0; z < 5; z++)
    {
        int j = 1;

        // if (!fork())
        // {

        // printf("%s", temp);
        bzero(buffer, SIZE);
        pthread_t id;

        pthread_create(&id, NULL, worker, &j);

        int *ptr;
        pthread_join(id, (void **)&ptr);

        // }
    }

    // close(connfd);
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("No Port Provided\n");
        exit(1);
    }
    int portno = atoi(argv[1]);
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;
    queue = queue_create(queuesize);

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
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(portno);

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (SA *)&servaddr, sizeof(servaddr))) != 0)
    {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");

    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0)
    {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
    len = sizeof(cli);

    while (1)
    {
        // Accept the data packet from client and verification
        connfd = accept(sockfd, (SA *)&cli, &len);
        if (connfd < 0)
        {
            printf("server accept failed...\n");
            exit(0);
        }
        else
            printf("server accept the client...\n");

        pthread_t thread;
        pthread_create(&thread, NULL, communicationThreadFunc, &connfd);
        pthread_detach(thread);
    }
}

void makefile(char *array, char *temp, ToSend file)
{

    FILE *fp;
    char ch;
    int i = 0;

    fp = fopen(temp, "r");
    if (fp == NULL)
    {
        strcpy(array, "No such file in Server Directory\n");
        perror("");
    }
    else
    {
        while (1)
        {
            ch = fgetc(fp);
            if (ch == EOF)
                break;
            array[i++] = ch;
        }
        fclose(fp);
    }
}

void listFiles(const char *path)
{
    struct dirent *dp;
    DIR *dir = opendir(path);

    // Unable to open directory stream
    if (!dir)
        return;

    while ((dp = readdir(dir)) != NULL)
    {
        printf("%s\n", dp->d_name);
    }

    // Close directory stream
    closedir(dir);
}

void getfile(char *array, char *temp, int *connfd)
{
    // printf("array: %s, temp: %s \n", array, temp);

    FILE *fp;
    char ch;
    int i = 0;

    // printf("------------%d----------", *(int *)queue_node_value(queue, queue_first(queue)));

    listFilesRecursively(temp, connfd);

    for (queueNode node = queue_first(queue); node != queue_EOF; node = queue_next(queue, node))
    {
        QueueFile val = (QueueFile)queue_node_value(queue, node);
        // printf("%d - %s \n", val->isDirectory, val->path);
    }
    // fp = fopen(temp, "r");
    // if (fp == NULL)
    // {
    //     strcpy(array, "No such file in Server Directory\n");
    //     perror("");
    // }
    // else
    // {
    //     while (1)
    //     {
    //         ch = fgetc(fp);
    //         if (ch == EOF)
    //             break;
    //         array[i++] = ch;
    //     }
    //     fclose(fp);
    // }
}