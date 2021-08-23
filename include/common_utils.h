/*
 * @Author: xdl
 * @Date: 2021-08-23 19:20:22
 * @LastEditTime: 2021-08-23 19:28:15
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /Talkroom/include/common_utils.h
 */

#include <stdio.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>

void addfd(int epoll_fd,int fd,bool enable_et);

int setNonBlock(int fd);
