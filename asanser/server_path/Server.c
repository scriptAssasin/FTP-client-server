/*

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

File: Server.c
Build: gcc Server.c -o Server
Execute: ./Server portnumber

*/


//Included Header Files
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<fcntl.h>
#define SIZE 500
#define QUIT_STRING "exit"
#define BLANK_STRING " "

void ls_pwd(char*,char*);
void getfile(char *,char *);
void makefile(char *,char *);

void error(char *msg)
{
    perror(msg);
    exit(1);
}

void copy(char b[],int beg,char t[])
{
    int i;
    for(i=beg; b[i]!='\n'; i++)
        t[i-beg]=b[i];
}

int main(int argc,char *argv[])
{
    int sockfd,newsockfd,portno,clientlen,ret,no_of_bytes;
    struct sockaddr_in serveraddr,clientaddr;
    char * command,* commandarray[40],buffer[SIZE],temp[SIZE];

    if(argc<2)
    {
        printf("No Port Provided\n");
        exit(1);
    }

    sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd<0)
        error("Error in socket");

    bzero((char *)&serveraddr,sizeof(serveraddr));
    portno=atoi(argv[1]);
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(portno);
    serveraddr.sin_addr.s_addr=INADDR_ANY;

    if(bind(sockfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr))<0)
        error("Error in binding");

    listen(sockfd,5);
    clientlen=sizeof(clientaddr);

    newsockfd=accept(sockfd,(struct sockaddr *)&clientaddr,&clientlen);
    if(newsockfd<0)
        error("Error in accepting");

    bzero(buffer,SIZE);

    while(1)
    {
        no_of_bytes=read(newsockfd,buffer,SIZE-1);

        if(no_of_bytes<0)
            error("Read");
        else if(no_of_bytes==0)
            printf("\nClient Disconnected\n");

        printf("\nMessage from Client: %s",buffer);

        if(strcmp(buffer,"s_ls\n")==0)
        {
            bzero(buffer,SIZE);
            ls_pwd(buffer,"ls");
            no_of_bytes=write(newsockfd,buffer,strlen(buffer));
        }
        else if(strcmp(buffer,"s_pwd\n")==0)
        {
            bzero(buffer,SIZE);
            ls_pwd(buffer,"pwd");
            no_of_bytes=write(newsockfd,buffer,strlen(buffer));
        }
        else if(strncmp(buffer,"get",3)==0)
        {
            bzero(temp,SIZE);
            copy(buffer,4,temp);
            bzero(buffer,SIZE);
            getfile(buffer,temp);
            no_of_bytes=write(newsockfd,buffer,strlen(buffer));
        }
//if put,then read again the file contents in buffer and save them in a file
        else if(strncmp(buffer,"put",3)==0)
        {
            bzero(temp,SIZE);
            copy(buffer,4,temp);
            bzero(buffer,SIZE);
            no_of_bytes=read(newsockfd,buffer,SIZE-1);
            if(strcmp(buffer,"No file found\n")!=0)
            {
                makefile(buffer,temp);
                bzero(buffer,SIZE);
                strcpy(buffer,"File created in Server directory.\n");
                no_of_bytes=write(newsockfd,buffer,strlen(buffer));
            }
            else
            {
                bzero(buffer,SIZE);
                strcpy(buffer,"No such file in Client Directory\n");
                no_of_bytes=write(newsockfd,buffer,strlen(buffer));
            }
        }
        else
        {
            bzero(buffer,SIZE);
            strcpy(buffer,"Recieved the message");
            no_of_bytes=write(newsockfd,buffer,strlen(buffer));
        }
        if(no_of_bytes<0)
            error("Write");
        bzero(buffer,SIZE);
    }
    return 0;
}

void makefile(char *array,char *temp)
{
    FILE *fp;
    char ch;
    int i=0;
    fp=fopen(temp,"w");
    while(1)
    {
        ch=array[i++];
        if(ch=='\0')
            break;
        fputc(ch,fp);
    }
    fclose(fp);
}

void getfile(char *array,char *temp)
{
    FILE *fp;
    char ch;
    int i=0;
    fp=fopen(temp,"r");
    if (fp==NULL)
    {
        strcpy(array,"No such file in Server Directory\n");
        perror("");
    }
    else
    {
        while(1)
        {
            ch=fgetc(fp);
            if(ch==EOF)
                break;
            array[i++]=ch;
        }
        fclose(fp);
    }
}

void ls_pwd(char* buffer,char* func )
{
    int fd,i=0;
    FILE *fp;
    char ch;

    fd = open("temp_file", O_WRONLY|O_CREAT|O_TRUNC, 0666);
    dup2(fd, 1);
    system(func);
    dup2(1,fd);
    close(fd);

    fp=fopen("temp_file","r");
    while(1)
    {
        ch=fgetc(fp);
        if(ch==EOF)
            break;
        buffer[i++]=ch;
    }
    fclose(fp);

}
