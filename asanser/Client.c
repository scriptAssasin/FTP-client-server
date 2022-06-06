/*

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

File: Client.c
Build: gcc Client.c -o Client
Execute: ./Client ipaddress portnumber

*/


//Included Header Files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#define SIZE 500

void getfile(char*,char *);
void makefile(char*,char *);

void error(char *msg)
{
    perror(msg);
    exit(0);
}

void copy(char b[],int beg,char t[])
{
    int i;
    for(i=beg; b[i]!='\n'; i++)
        t[i-beg]=b[i];
}

int main(int argc, char *argv[])
{
    int sockfd, portno, no_of_bytes;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char buffer[SIZE],temp[SIZE];

    if (argc < 3)
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

    bzero((char *)&serveraddr,sizeof(serveraddr));
    serveraddr.sin_family=AF_INET;
    bcopy((char *)server->h_addr,(char *)&serveraddr.sin_addr.s_addr,sizeof(server->h_addr));
    serveraddr.sin_port=htons(portno);

    if(connect(sockfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr))<0)
        error("Error in connecting the socket");

    while(1)
    {
        bzero(buffer,SIZE);
        printf("768/IT/11_ftp-client> ");
        fgets(buffer,SIZE-1,stdin);

        if(strcmp(buffer,"quit\n")==0)
        {
            close(sockfd);
            return 0;
        }
        else if(strcmp(buffer,"c_pwd\n")==0)
        {
            printf("Message from Server:\n\n");
            system("pwd");
            printf("\n");
        }
        else if(strcmp(buffer,"c_ls\n")==0)
        {
            printf("Message from Server:\n\n");
            system("ls");
            printf("\n");
        }
        else
        {

            no_of_bytes=write(sockfd,buffer,strlen(buffer));
            if(no_of_bytes<0)
            {
                error("Error in writing");
                return 1;
            }

            //if put has been sent then read file contents in buffer and again write them to server
            if(strncmp(buffer,"put",3)==0)
            {
                bzero(temp,SIZE);
                copy(buffer,4,temp);
                getfile(buffer,temp);
                no_of_bytes=write(sockfd,buffer,strlen(buffer));
                if(no_of_bytes<0)
                {
                    error("Error in writing");
                    return 1;
                }
                printf("Message from Server:\n\n");
                bzero(buffer,SIZE);
                no_of_bytes = read(sockfd,buffer,SIZE-1);
                if (no_of_bytes < 0)
                {
                    error("Error in reading");
                    break;
                }
                printf("%s\n",buffer);
            }

            else if(strncmp(buffer,"get",3)==0)
            {
                bzero(temp,SIZE);
                copy(buffer,4,temp);

                printf("Message from Server:\n\n");
                bzero(buffer,SIZE);
                no_of_bytes = read(sockfd,buffer,SIZE-1);
                if (no_of_bytes < 0)
                {
                    error("Error in reading");
                    break;
                }

                if(strcmp(buffer,"No such file in Server Directory\n")!=0)
                {
                    makefile(buffer,temp);
                    bzero(buffer,SIZE);
                    printf("File created in Client directory.\n");
                }
                else
                {
                    bzero(buffer,SIZE);
                    printf("No such file in Server Directory.\n");
                }
            }

            else
            {
                printf("Message from Server:\n\n");
                bzero(buffer,SIZE);
                no_of_bytes = read(sockfd,buffer,SIZE-1);
                if (no_of_bytes < 0)
                {
                    error("Error in reading");
                    break;
                }
                printf("%s\n",buffer);
            }
        }
    }
}

void getfile(char *array,char *temp)
{
    FILE *fp;
    char ch;
    int i=0;
    fp=fopen(temp,"r");
    bzero(array,SIZE);
    if (fp==NULL)
    {
        strcpy(array,"No file found\n");
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
