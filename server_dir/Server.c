// EVANGELOS FAKORELLIS, sdi1900203

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

// this type of struct is stored in every queue node
typedef struct queueFile
{
    bool isDirectory;
    char path[512];
    int socket_file_descriptor;
} queueFile;

typedef queueFile *QueueFile;

// this type of struct is used for data sending between server and client. Gives information about the file if it is directory or file, the relative path and the content
typedef struct transferInfo
{
    bool isDirectory;
    char path[512];
    char content[512];
} transferInfo;

typedef transferInfo *TransferInfo;

// thread pool initialized
pthread_t workerthreads[100];
// Our queue
Queue queue;

// When a connection is created, a communication thread is created too. This function is assigned to the communication thread created
void* communicationThreadFunc(void *);
// scan directory, subdirectories and files recursively and append to queue
void scanDirectory(char *, int *socketfd);
// read a file character to character from a specific path, and append every character to the given array
void readFile(char *, char *);
// Worker Function given as parameter to worker threads, in order to scan the queue and send data to client
void* WorkerFunction(void *);
// helper function to understand if file is directory or just file
int isFolderorFile(const char *);


int main(int argc, char **argv)
{
    // command line arguments validation check
    if (argc < 8 || strcmp(argv[1], "-p") != 0 || strcmp(argv[3], "-s") != 0 || strcmp(argv[5], "-q") != 0 || strcmp(argv[7], "-b") != 0)
    {
        printf("Wrong command line configuration \n");
        exit(1);
    }

    int portno = atoi(argv[2]);
    int queuesize = atoi(argv[6]);

    int sockfd, socketfd, len;
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
        printf("Server was successfully initialized...\n");

    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(portno);

    // Binding newly created socket to given IP and port
    if ((bind(sockfd, (SA *)&servaddr, sizeof(servaddr))) != 0)
    {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");

    // Now server is ready to listen for connections to given port number
    if ((listen(sockfd, 5)) != 0)
    {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Listening for connections to port %d\n", portno);

    len = sizeof(cli);

    while (1)
    {
        // Accept the connection from client
        socketfd = accept(sockfd, (SA *)&cli, &len);
        if (socketfd < 0)
        {
            printf("server accept failed...\n");
            exit(0);
        }
        else
            printf("Accepted connection from localhost\n");

        pthread_t thread;
        pthread_create(&thread, NULL, communicationThreadFunc, &socketfd);
        pthread_detach(thread);
    }
}

void readFile(char *array, char *path)
{

    FILE *fp;
    char ch;
    int i = 0;

    fp = fopen(path, "r");

    if (fp == NULL)
        perror("");

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

void scanDirectory(char *basePath, int *socketfd)
{
    // get thread_self in order to get thread id
    pthread_t self;
    self = pthread_self();

    char path[1000];
    struct dirent *dp;
    DIR *dir = opendir(basePath);

    // Unable to open directory
    if (!dir)
        return;

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            // create a QueueFile object
            QueueFile file = malloc(sizeof(*file));

            file->isDirectory = (bool)isFolderorFile(path);
            // Create relative path
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);
            
            printf("[Thread: %ld]: Adding file %s to the queue... \n", self, path);

            strcpy(file->path, path);
            file->socket_file_descriptor = *socketfd;
            // append QueueFile object to the Queue
            queue_insert(queue, file);
            // recursively scan directory and subdirectories
            scanDirectory(path, socketfd);
        }
    }

    closedir(dir);
}

void *WorkerFunction(void *p)
{
    int no_of_bytes;
    char buffer[SIZE];
    bzero(buffer, SIZE); 

    // iterate our queue
    for (queueNode node = queue_first(queue); node != queue_EOF; node = queue_next(queue, node))
    {
        QueueFile val = (QueueFile)queue_node_value(queue, node);

        // if is just file, NOT a directory
        if (val->isDirectory == 0)
        {
            // create a new TransferInfo object 
            TransferInfo file = malloc(sizeof(*file));
            file->isDirectory = val->isDirectory;
            strcpy(file->path, val->path);

            pthread_t self;
            self = pthread_self();
            printf("[Thread: %ld]: About to read file %s \n", self, val->path);

            // add file contents to buffer
            readFile(buffer, val->path);
            strcpy(file->content, buffer);

            no_of_bytes = write(val->socket_file_descriptor, file, sizeof(*file));

            if (no_of_bytes < 0)
                perror("Write");

            bzero(buffer, SIZE);
            bzero(file->content, SIZE);
        }
        else
        {
            TransferInfo file = malloc(sizeof(*file));
            file->isDirectory = val->isDirectory;
            strcpy(file->path, val->path);

            no_of_bytes = write(val->socket_file_descriptor, file, sizeof(*file));

            if (no_of_bytes < 0)
                perror("Write");

            bzero(buffer, SIZE);
            bzero(file->content, SIZE);
        }
    }
}

int isFolderorFile(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

void *communicationThreadFunc(void *socketfd)
{
    int no_of_bytes;
    char buffer[SIZE], temp[SIZE];
    bzero(buffer, SIZE);

    no_of_bytes = read(*(int *)socketfd, buffer, SIZE - 1);

    if (no_of_bytes < 0)
        perror("Read");

    else if (no_of_bytes == 0)
        printf("\nClient Disconnected\n");

    printf("\nAbout to scan directory: %s \n", buffer);

    bzero(temp, SIZE);
    strcpy(temp, buffer);
    scanDirectory(temp, (int *)socketfd);

    // fork in order to support multiclent
    if (!fork())
    {

        bzero(buffer, SIZE);
        // create a worker thread and add WorkerFunction to it
        pthread_create(&workerthreads[0], NULL, WorkerFunction, &queue);

        int *ptr;
        pthread_join(workerthreads[0], (void **)&ptr);
    }

    close(*(int *)socketfd);
}