#include<stdio.h>
#include<ctype.h>
#include<unistd.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<stdlib.h>
#include <fcntl.h>


#define MAX 100
#define PORT 43454
#define SA struct sockaddr

#define OUTFIFO  "client_to_server_fifo"
#define INFIFO "server_to_client_fifo"

int clientCount = 0;
char responseText[100],clientName[100];

struct ChatClient
{
    pid_t pid;
    char name[100];
    char time[100];
    char infifoname[100];
    char outfifoname[100];
    int infifo;
    int outfifo;
    int ConnStaus;
    
}clientList[5];
struct ChatClient currentClient;

int openConnection(struct ChatClient *source,struct ChatClient *destn)
{   
    printf("%d",destn->ConnStaus);
    if(!(destn->ConnStaus > 0))
    { printf("i'm hereds\n");
        return -1;
    }else{
        /* create the FIFO (named pipe) */
        printf("%s-->%s\n",source->infifoname,source->outfifoname);
        
        int f1 = mkfifo(source->infifoname, 0667);
        int f2 = mkfifo(source->outfifoname, 0667);
        
        if(f1<0 || f2<0)
        {
            printf("Error in creating FIFO\n");
            return -1;
        }
        
        source->infifo = open(source->infifoname,O_RDONLY);
        source->outfifo = open(source->outfifoname,O_WRONLY);
        
        if(source->infifo<0 || source->outfifo<0)
        {
            printf("Error in opening FIFO\n");
            return -1;
        }
        return 1;
    }
}
void func(int sockfd)
{
    struct ChatClient destnClient;
    char buff[MAX];
    int n;
    
    bzero(buff,MAX);
    
    read(sockfd,buff,sizeof(buff));

    char destnClientName[100];
    printf("%s client has requested connection to %s\n",currentClient.name,buff);
    sprintf(destnClientName,"%s",buff);
    
    int j,found=-1;

    for(j=0;j<=clientCount;j++)
    {
        if(strcmp(clientList[j].name,destnClientName)==0)
        {   
            //printf("%s is online, connecting",destnClientName);
            
            sprintf(destnClient.name,"%s",clientList[j].name);
            sprintf(destnClient.outfifoname,"%s",clientList[j].outfifoname);
            sprintf(destnClient.infifoname,"%s", currentClient.outfifoname);
            sprintf(currentClient.infifoname,"%s", destnClient.outfifoname);
            printf("Establishing connection: %s to %s\n",currentClient.name,destnClient.name);
            found =1;
            break;
            
        }
    }
    if(found<0)
    {
        bzero(buff,MAX);
        sprintf(buff,"%s is offline",destnClient.name);
        write(sockfd,buff,sizeof(buff));
        return ;
    }
        
    int conn = openConnection(&currentClient,&destnClient);
    if(conn>0)
    {

        bzero(buff,MAX);
        sprintf(buff,"Connected to %s",destnClient.name);
        write(sockfd,buff,sizeof(buff));
        
    }
    for(;;)
    {
        bzero(buff,MAX);
        read(sockfd,buff,sizeof(buff));       
        
        printf("%s: %s",currentClient.name,buff);
        printf("%s->%s:",destnClient.name,clientName);
        bzero(buff,MAX);
        n=0;

        while((buff[n++]=getchar())!='\n');
        write(sockfd,buff,sizeof(buff));
        if(strncmp("exit",buff,4)==0)
        {
            printf("Server Exit...\n");
            break;
        }
    }
}


int main()
{
    int sockfd,connfd,len,n,pid;
    struct sockaddr_in servaddr,cli;
    int infifo,outfifo;
    
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
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(PORT);
   
    if((bind(sockfd,(SA*)&servaddr, sizeof(servaddr)))!=0)
    {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");
    
    if((listen(sockfd,5))!=0)
    {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
     
    len=sizeof(cli);
    while(1){
       
        connfd=accept(sockfd,(SA *)&cli,&len);

        if(connfd<0)
        {
            printf("server accept failed...\n");
            exit(0);
        }
        else if(clientCount<5)
        {
            time_t mytime;
            char clientTime[100];
            
            bzero(responseText,100);
            clientCount++;
            
            mytime = time(NULL);
            sprintf(clientTime,"%s",ctime(&mytime));
            char *tmp;
            tmp=strchr(clientTime,'\n');
            strcpy(tmp,"\0");
            
            printf("________________________________________________\n");
            printf("<CLIENT_%d> online at %s\n",clientCount,clientTime);
            printf("________________________________________________\n");
            
            strncpy(responseText,"TRUE",6);
            n = write(connfd,responseText,6); // Send the response to the client
            
            bzero(responseText,100);
            sprintf(responseText,"CLIENT_%d|%30s",clientCount,clientTime);
            sprintf(clientName,"CLIENT_%d",clientCount);
            n = write(connfd,responseText,strlen(responseText)); // Send the response to the client
            
            bzero(responseText,100);
            sprintf(responseText,"%d",clientCount);
            n = write(connfd,responseText,strlen(responseText));
            
            
            int j;
            
            for(j=0;j<clientCount;j++)
            {
                bzero(responseText,100);
                sprintf(responseText,"%s:%s",clientList[j].name,clientList[j].time);
                n = write(connfd,responseText,strlen(responseText));
            }    
            
            //mkfifo(OUTFIFO, 0666);
            //mkfifo(INFIFO, 0666);
            
            char outfifoname[100],infifoname[100];
            sprintf(outfifoname,"%s%d",OUTFIFO,clientCount);
            //infifo = open(infifoname, O_RDONLY);
            //outfifo = open(outfifoname, O_WRONLY);
            
            //sprintf(clientList[clientCount].infifoname,"%s",infifoname);
            sprintf(clientList[clientCount].outfifoname,"%s",outfifoname);
            clientList[clientCount].ConnStaus = 0;
            strcpy(clientList[clientCount].name,clientName);
            strcpy(clientList[clientCount].time,clientTime);
            
            for(j=0;j<clientCount;j++)
            {          //  bzero(responseText,100);
                printf("%8s|%30s|%d\n",clientList[j].name,clientList[j].time,getpid());    
            }
            
        }else
        {
            strncpy(responseText,"FALSE",6);
            n = write(connfd,responseText,6); // Send the response to the client
            close(connfd);
            continue;
        }
       
        pid = fork();

        if (pid < 0) {
         perror("ERROR on fork");
         exit(1);
        }
        if(pid == 0) {
            
            
            clientList[clientCount].pid = getpid();
            //printf("processing client %d\n",clientList[clientCount].pid);
            currentClient = clientList[clientCount];
           // printf("processing client %d\n",currentClient.pid);
            func(connfd);
        }
        
       // clientList[clientCount].pid = pid;
        //printf("%d",pid);
        //int j;
        
        /*for(j=0;j<clientCount;j++)
        {
          //  bzero(responseText,100);
            printf("%8s|%30s|%d\n",clientList[j].name,clientList[j].time,clientList[j].pid);    
        }*/
    }
    close(sockfd);
    return 0;
}