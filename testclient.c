#include<stdio.h>
#include<ctype.h>
#include<unistd.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<stdlib.h>

#define MAX 80
#define PORT 43454
#define SA struct sockaddr

int clientCount;

struct ChatClient
{
    pid_t pid;
    char name[100];
    char time[100];
    
}clientList[5];

void func(int sockfd)
{
    char buff[MAX];
    int n;
    
    for(;;)
    {
        bzero(buff,sizeof(buff));
        printf("Me : ");
        n=0;
        
        while((buff[n++]=getchar())!='\n');
        
        write(sockfd,buff,sizeof(buff));
        bzero(buff,sizeof(buff));
        read(sockfd,buff,sizeof(buff));
        printf("Sender: %s",buff);
        
        if((strncmp(buff,"exit",4))==0)
        {
            printf("Client Exit...\n");
            break;
        }
    }
}

int main()
{
    int sockfd,connfd,n;
    char buffer[100];
    struct sockaddr_in servaddr,cli;
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd==-1)
    {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=inet_addr("127.0.0.1");
    servaddr.sin_port=htons(PORT);
    
    if(connect(sockfd,(SA *)&servaddr,sizeof(servaddr))!=0)
    {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else{
        bzero(buffer,100);
        n = read(sockfd,buffer,100);
        if (n < 0) 
            error("ERROR reading from socket");
        else
        {   if(strcmp(buffer,"FALSE")==0)
            {  
                printf("Connection refused:\n");
                close(sockfd);
                return 0;
            }else{
                printf("Connected to server:\n");
                bzero(buffer,100);
                n = read(sockfd,buffer,100);
                printf("______________________________________________________________________________________\n");
                printf("%s\n",buffer); 
                printf("______________________________________________________________________________________\n");
                bzero(buffer,100);
                
                n = read(sockfd,buffer,100);
                printf("No. of clients online:%s\n",buffer);
                printf("______________________________________________________________________________________\n");
                clientCount = atoi(buffer);
               
                printf("%6s|%10s|%30s\n","Ser no.","Name","time");
                printf("______________________________________________________________________________________\n");
                               
                int j=0;
                j=0;
                for(;j<clientCount;j++)
                {
                     bzero(buffer,100);
                     n = read(sockfd,buffer,100);
                     printf("%6d %s\n",j+1,buffer);
                }
                printf("______________________________________________________________________________________\n");
                
                printf("Enter the client name:");
                char destnClient[100];
                scanf("%s",destnClient);
                int n;
                n = write(sockfd,destnClient,sizeof(destnClient));
            }
        }
    }
  
    func(sockfd);
    close(sockfd);
    
    return 0;
}
