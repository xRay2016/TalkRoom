/*
 * @Author: xdl
 * @Date: 2021-08-23 19:23:15
 * @LastEditTime: 2021-08-23 19:30:49
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /Talkroom/src/common_utils.cpp
 */
#include "common_utils.h"

void addfd(int epoll_fd,int fd,bool enable_et)
{
    epoll_event event;
    event.data.fd=fd;
    event.events=EPOLLIN;
    if(enable_et)
    {
        event.events|=EPOLLET;
    }
    epoll_ctl(epoll_fd,EPOLL_CTL_ADD,fd,&event);
    setNonBlock(fd);
}

int setNonBlock(int fd)
{
    int old_option=fcntl(fd,F_GETFL);
    int new_option=old_option|O_NONBLOCK;
    fcntl(fd,F_SETFL,new_option);
    return old_option;
}
