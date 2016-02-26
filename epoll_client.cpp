#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#include "common.h"

#define MAXSIZE 1024
#define IPADDRESS "127.0.0.1"
#define SERVER_PORT 9900
#define FDSIZE 1024
#define EPOLLEVENTS 30

static void handle_connection(int sockfd);

static void handle_events(int epollfd, struct epoll_event* events, int num, int sockfd, char* buf);

static void do_read(int epollfd, int fd, int sockfd, char* buf);

static void do_write(int epollfd, int fd, int sockfd, char* buf);


int main(int argc, char* argv[]){
	int sockfd, errno;
	struct sockaddr_in servaddr;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1){
		fprintf(stderr, "Create socket fail:%s\n", strerror(errno));
		exit(1);
	}
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, IPADDRESS, &servaddr.sin_addr);
	if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
		fprintf(stderr, "Connect to server fail:%s\n", strerror(errno));
		close(sockfd);
		exit(1);
	}
	handle_connection(sockfd);
}

static void handle_connection(int sockfd){
	int epollfd;
	struct epoll_event events[EPOLLEVENTS];
	char buf[MAXSIZE];
	int ret;
	epollfd = epoll_create(FDSIZE);
	if(epollfd == -1){
		fprintf(stderr, "Create epoll events handle faile:%s\n", strerror(errno));
		close(sockfd);
		exit(1);
	}
	add_event(epollfd, STDIN_FILENO, EPOLLIN);
	for(;;){
		ret = epoll_wait(epollfd, events, EPOLLEVENTS, -1);
		handle_events(epollfd, events, ret, sockfd, buf);
	}
}

static void handle_events(int epollfd, struct epoll_event* events, int num, int sockfd, char* buf){
	int fd;
	for(int i = 0; i < num; i++){
		fd = events[i].data.fd;
		if(events[i].events & EPOLLIN){
			do_read(epollfd, fd, sockfd, buf);
		} else if(events[i].events & EPOLLOUT){
			do_write(epollfd, fd, sockfd, buf);
		}
	}
}

static void do_read(int epollfd, int fd, int sockfd, char* buf){
	int nread, errno;
	memset(buf, 0, MAXSIZE);
	nread = read(fd, buf, MAXSIZE);
	if(nread == 0){
		fprintf(stderr, "Server close!");
		close(fd);
		exit(1);
	} else if(nread == -1){
		fprintf(stderr, "read data failed:%s\n", strerror(errno));
		close(fd);
		exit(1);
	}else {
		if(fd == STDIN_FILENO){
			add_event(epollfd, sockfd, EPOLLOUT);
		} else {
			delete_event(epollfd, sockfd, EPOLLIN);
			add_event(epollfd, STDOUT_FILENO, EPOLLOUT);
		}
	}
}

static void do_write(int epollfd, int fd, int sockfd, char* buf){
	int nwrite, errno;
	nwrite = write(fd, buf, strlen(buf));
	if(nwrite == -1){
		fprintf(stderr, "write data faile:%s\n", strerror(errno));
		close(fd);
	} else {
		if(fd == STDOUT_FILENO){
			delete_event(epollfd, fd, EPOLLOUT);
		} else {
			modify_event(epollfd, fd, EPOLLIN);
		}
	}
	memset(buf, 0, MAXSIZE);
}

