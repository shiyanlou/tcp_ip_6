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
    int sockfd, newfd;
    struct sockaddr_in s_addr, c_addr;
    char buf[BUFLEN];
    socklen_t len;
    unsigned int port, listnum;
    
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
    /*设置侦听队列长度*/
    if(argv[3])
        listnum = atoi(argv[3]);
    else
        listnum = 3;
    /*设置服务器ip*/
    bzero(&s_addr, sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(port);
    if(argv[1])
        s_addr.sin_addr.s_addr = inet_addr(argv[1]);
    else
        s_addr.sin_addr.s_addr = INADDR_ANY;
    /*把地址和端口帮定到套接字上*/
    if((bind(sockfd, (struct sockaddr*) &s_addr,sizeof(struct sockaddr))) == -1){
        perror("bind");
        exit(errno);
    }
    /*侦听本地端口*/
    if(listen(sockfd,listnum) == -1){
        perror("listen");
        exit(errno);    
    }
    while(1){
        printf("*****************server start***************\n");
        len = sizeof(struct sockaddr);
        if((newfd = accept(sockfd,(struct sockaddr*) &c_addr, &len)) == -1){
            perror("accept");        
            exit(errno);
        }
        while(1){
        _retry:
            /******发送消息*******/
            bzero(buf,BUFLEN);
            printf("enter your words:");
            /*fgets函数：从流中读取BUFLEN-1个字符*/
            fgets(buf,BUFLEN,stdin);
            /*打印发送的消息*/
            //fputs(buf,stdout);
            if(!strncasecmp(buf,"quit",4)){
                printf("server stop\n");
                break;
            }
            /*如果输入的字符串只有"\n"，即回车，那么请重新输入*/
            if(!strncmp(buf,"\n",1)){
                
                goto _retry;
            }    
            /*如果buf中含有'\n'，那么要用strlen(buf)-1，去掉'\n'*/            
            if(strchr(buf,'\n'))
                len = send(newfd,buf,strlen(buf)-1,0);
            /*如果buf中没有'\n'，则用buf的真正长度strlen(buf)*/    
            else
                len = send(newfd,buf,strlen(buf),0);
            if(len > 0)
                printf("send successful\n");            
            else{
                printf("send failed\n");
                break;            
            }
            /******接收消息*******/
            bzero(buf,BUFLEN);
            len = recv(newfd,buf,BUFLEN,0);
            if(len > 0)
                printf("receive massage:%s\n",buf);
            else{
                if(len < 0 )
                    printf("receive failed\n");
                else
                    printf("client stop\n");
                break;        
            }
        }
        /*关闭聊天的套接字*/
        close(newfd);
        /*是否退出服务器*/
        printf("exit?：y->yes；n->no ");
        bzero(buf, BUFLEN);
        fgets(buf,BUFLEN, stdin);
        if(!strncasecmp(buf,"y",1)){
            printf("server stop\n");
            break;
        }
    }
    /*关闭服务器的套接字*/
    close(sockfd);
    return 0;
}
