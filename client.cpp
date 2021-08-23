/*
 * @Author: your name
 * @Date: 2021-08-23 19:20:02
 * @LastEditTime: 2021-08-23 20:07:06
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /Talkroom/client.cpp
 */
#include "common_utils.h"

#define BUFFER_SIZE 1024

int main(int argc,char* argv[])
{
    const char* ip=argv[1];
    int port=atoi(argv[2]);
    char message[BUFFER_SIZE];
    
    sockaddr_in server_addr;
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(port);
    server_addr.sin_addr.s_addr=inet_addr(ip);

    int sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(connect(sockfd,(sockaddr*)&server_addr,sizeof(server_addr))<0)
    {
        printf("connection error\n");
        exit(-1);
    }
    char sendline[1024];
    char revline[1024];
    int pipe_fd[2];
    if(pipe(pipe_fd)<0)
    {
        printf("pipe error\n");
        exit(-1);
    }
    int epoll_fd=epoll_create(5);
    addfd(epoll_fd,sockfd,true);
    addfd(epoll_fd,pipe_fd[0],true);
    epoll_event events[2];

    pid_t pid=fork();
    int nready=0;
    bool isClientwork=true;
    if(pid<0)
    {
        printf("create process error\n");
        exit(-1);
    }
    else if(pid>0)
    {
        //主进程,关闭写端
        close(pipe_fd[1]);
        while(isClientwork)
        {
            nready=epoll_wait(epoll_fd,events,2,-1);
            for(int i=0;i<nready;i++)
            {
                memset(message,0,BUFFER_SIZE);
                if(events[i].data.fd==sockfd)
                {
                    if(recv(sockfd,message,BUFFER_SIZE,0)<=0)
                    {
                        printf("server error\n");
                        isClientwork=false;
                        close(sockfd);
                    }
                    else
                    {
                        printf("%s\n",message);
                    }
                }
                else
                {
                    if(read(events[i].data.fd,message,BUFFER_SIZE)==0)
                    {
                        printf("parent read pipe fail\n");
                        isClientwork = false;
                    }
                    else
                    {
                        send(sockfd, message, strlen(message), 0); 
                    }
                }
            }
        }
        
    }
    else
    {
        //子进程，关闭读端
        close(pipe_fd[0]);
        while (isClientwork) {
            bzero(&message, BUFFER_SIZE);
            //从标准输入读取数据
            fgets(message, BUFFER_SIZE, stdin);
            //判断是否输入了'exit'
            if (strncasecmp(message, "exit", 4) == 0) {
                isClientwork = false;    
            } else {
                //从标准输入读取的数据写入管道    
                if (write(pipe_fd[1], message, strlen(message) - 1) < 0) {
                    printf("client write pip fail.\n");
                    exit(-1);
                }
            }
        }
    }
    
    if (pid > 0) {
        close(pipe_fd[0]);
        close(sockfd);
    } else {
        close(pipe_fd[1]);    
    }
}