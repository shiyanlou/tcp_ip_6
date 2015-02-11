#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFLEN 10

int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in s_addr;
    socklen_t len;
    unsigned int port;
    char buf[BUFLEN];    
    
    /*建立socket*/
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("socket");
        exit(errno);
    }
    /*设置服务器端口*/    
    if(argv[2])
        port = atoi(argv[2]);
    else
        port = 7777;
    /*设置服务器ip*/
    bzero(&s_addr, sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(port);
    if (inet_aton(argv[1], (struct in_addr *)&s_addr.sin_addr.s_addr) == 0) {
        perror(argv[1]);
        exit(errno);
    }
    /*开始连接服务器*/    
    if(connect(sockfd,(struct sockaddr*)&s_addr,sizeof(struct sockaddr)) == -1){
        perror("connect");
        exit(errno);
    }else
        printf("*****************client start***************\n");
    
    while(1){
        /******接收消息*******/
        bzero(buf,BUFLEN);
        len = recv(sockfd,buf,BUFLEN,0);
        if(len > 0)
            printf("receive massage:%s\n",buf);
        else{
            if(len < 0 )
                printf("receive failed\n");
            else
                printf("server stop\n");
            break;    
        }
    _retry:    
        /******发送消息*******/    
        bzero(buf,BUFLEN);
        printf("enter your words:");
        /*fgets函数：从流中读取BUFLEN-1个字符*/
        fgets(buf,BUFLEN,stdin);
        /*打印发送的消息*/
        //fputs(buf,stdout);
        if(!strncasecmp(buf,"quit",4)){
            printf("client stop\n");
            break;
        }
        /*如果输入的字符串只有"\n"，即回车，那么请重新输入*/
        if(!strncmp(buf,"\n",1)){
            
            goto _retry;
        }
        /*如果buf中含有'\n'，那么要用strlen(buf)-1，去掉'\n'*/    
        if(strchr(buf,'\n'))
            len = send(sockfd,buf,strlen(buf)-1,0);
        /*如果buf中没有'\n'，则用buf的真正长度strlen(buf)*/    
        else
            len = send(sockfd,buf,strlen(buf),0);
        if(len > 0)
            printf("send successful\n");            
        else{
            printf("send failed\n");
            break;            
        }
    }
    /*关闭连接*/
    close(sockfd);

    return 0;
}
