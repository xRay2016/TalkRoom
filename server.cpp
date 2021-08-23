/*
 * @Author: xdl
 * @Date: 2021-08-23 19:19:42
 * @LastEditTime: 2021-08-23 20:24:02
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /Talkroom/server.cpp
 */
#include "common_utils.h"
#include <list>

#define USER_LIMIT 5
#define BUFFER_SIZE 64
#define FD_LIMIT 65535
#define MAX_EVENT_NUMBER 1024
#define MESSAGE_FORMAT "Client %d say: %s"

std::list<int> client_vec;

/**
 * @description: 
 * @param {int} sockfd 发送的client连接描述符
 * @param {char*} msg 发送的消息
 * @return {*}
 */
void boardcastMessage(int sockfd,char* msg)
{
    auto iter=client_vec.begin();
    for(;iter!=client_vec.end();iter++)
    {
        if(sockfd!=*iter)
        {
            if(send(*iter,msg,BUFFER_SIZE,0)<0)
            {
                printf("send to %d error\n",*iter);
                exit(-1);
            }
        }
    }
}

void ET(epoll_event* events,int number,int epoll_fd,int listen_fd)
{
    char buff[BUFFER_SIZE],msg[BUFFER_SIZE];
    for(int i=0;i<number;i++)
    {
        int sockfd=events[i].data.fd;
        if(sockfd==listen_fd)
        {
            sockaddr_in client_addr;
            socklen_t addr_len=sizeof(client_addr);
            int clientfd=accept(listen_fd,(sockaddr*)&client_addr,&addr_len);
            addfd(epoll_fd,clientfd,true);
            client_vec.push_back(clientfd);
            printf("Create connection %d\n",clientfd);
        }
        else if(events[i].events&EPOLLIN)
        {
            printf("event trigger once\n");
            while(1)
            {
                memset(buff,0,BUFFER_SIZE);
                int ret=recv(sockfd,buff,BUFFER_SIZE-1,0);
                if(ret<0)
                {
                    if(errno==EAGAIN||errno==EWOULDBLOCK)
                    {
                        printf("read later\n");
                        break;
                    }
                    printf("close connection\n");
                    close(sockfd);
                    client_vec.remove(sockfd);
                    break;
                }
                else if(ret==0)
                {
                    printf("close connection\n");
                    close(sockfd);
                    client_vec.remove(sockfd);
                }
                else
                {
                    printf("get %d bytes of content:%s \n",ret,buff);
                    sprintf(msg,MESSAGE_FORMAT,sockfd,buff);
                    boardcastMessage(sockfd,msg);
                }
            }
        }
        else
        {
            printf("something else happened.\n");
        }
    }
}

int main(int argc, char* argv[])
{
    const char* ip=argv[1];
    int port=atoi(argv[2]);

    sockaddr_in server_addr;
    bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family=AF_INET;
    server_addr.sin_addr.s_addr=inet_addr(ip);
    server_addr.sin_port=htons(port);

    int listenfd=socket(AF_INET,SOCK_STREAM,0);
    if(listenfd<0)
    {
        printf("socket error\n");
        return -1;
    }
    if(bind(listenfd,(sockaddr*)&server_addr,sizeof(server_addr))<0)
    {
        printf("bind error\n");
        return -1;
    }
    if(listen(listenfd,5)<0)
    {
        printf("listen error\n");
        return -1;
    }
    int user_count=0;

    int epoll_fd=epoll_create(5);
    addfd(epoll_fd,listenfd,true);

    int nready=0;
    epoll_event events[MAX_EVENT_NUMBER];

    while(1)
    {
        nready=epoll_wait(epoll_fd,events,MAX_EVENT_NUMBER,-1);
        if(nready<0)
        {
            printf("epoll error\n");
            break;
        }
        ET(events,nready,epoll_fd,listenfd);   
    }
    close(listenfd);
    return 0;

}