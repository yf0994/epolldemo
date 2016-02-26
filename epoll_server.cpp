#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#include "common.h"

#define IPADDRESS "127.0.0.1"
#define PORT 9900
#define MAXSIZE 1024
#define LISTENQ 5
#define FDSIZE 1000
#define EPOLLEVENTS 100

static int socket_bind(const char* ip, int port);

static void do_epoll(int listenfd);

static void handle_events(int epollfd, struct epoll_event* events, int num, int listenfd, char* buf);

static void handle_accept(int epollfd, int listenfd);

static void do_read(int epollfd, int fd, char* buf);

static void do_write(int epollfd, int fd, char* buf);

int main(int argc, char* argv[]){
	int listenfd;
	listenfd = socket_bind(IPADDRESS, PORT);
	if(listenfd == -1){
		exit(1);
	}

	if(listen(listenfd, LISTENQ) == -1){
		fprintf(stderr, "%s\n", strerror(errno));
		close(listenfd);
		exit(1);
	}

	do_epoll(listenfd);
}

static int socket_bind(const char* ip, int port){
	int sockfd;
	struct sockaddr_in servaddr;
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		return -1;
	}
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_port = htons(port);
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &servaddr.sin_addr);
	if(bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
		fprintf(stderr, "%s\n", strerror(errno));
		close(sockfd);
		exit(1);
	}
	return sockfd;
}

static void do_epoll(int listenfd){
	int epollfd, errno;
	struct epoll_event events[EPOLLEVENTS];
	int ret;
	char buf[MAXSIZE];
	memset(buf, 0, MAXSIZE);
	epollfd = epoll_create(FDSIZE);
	if(epollfd == -1){
		fprintf(stderr, "%s\n", strerror(errno));
	}
	add_event(epollfd, listenfd, EPOLLIN);
	for(; ;){
		ret = epoll_wait(epollfd, events, EPOLLEVENTS, -1);
		handle_events(epollfd, events, ret, listenfd, buf);
	}

	close(epollfd);
}

static void handle_events(int epollfd, struct epoll_event* events, int num, int listenfd, char* buf){
	int i, fd;
	for(i = 0; i < num; i++){
		fd = events[i].data.fd;
		if((fd == listenfd) && (events[i].events & EPOLLIN)){
			handle_accept(epollfd, listenfd);
		} else if(events[i].events & EPOLLIN){
			do_read(epollfd, fd, buf);
		} else if(events[i].events & EPOLLOUT){
			do_write(epollfd, fd, buf);
		}
	}
}

static void handle_accept(int epollfd, int listenfd){
	int clientfd;
	struct sockaddr_in clientaddr;
	socklen_t clientaddrlen = sizeof(clientaddr);
	clientfd = accept(listenfd, (struct sockaddr*)&clientaddr, &clientaddrlen);
	if(clientfd == -1){
		fprintf(stderr, "%s\n", strerror(errno));
		return;
	}
	printf("Accept a new client: %s:%d\n", inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port);
	add_event(epollfd, clientfd, EPOLLIN);
}

static void do_read(int epollfd, int fd, char* buf){
	int nread;
	nread = read(fd, buf, MAXSIZE);
	if(nread == -1){
		fprintf(stderr, "%s\n", strerror(errno));
		close(fd);
		delete_event(epollfd, fd, EPOLLIN);
	} else if(nread == 0){
		fprintf(stderr, "Client close.\n");
		close(fd);
		delete_event(epollfd, fd, EPOLLIN);
	} else {
		printf("read message is : %s", buf);
		modify_event(epollfd, fd, EPOLLOUT);
	}
}

static void do_write(int epollfd, int fd, char* buf){
	int nwrite;
	nwrite = write(fd, buf, strlen(buf));
	if(nwrite == -1){
		fprintf(stderr, "%s\n", strerror(errno));
		close(fd);
		delete_event(epollfd, fd, EPOLLOUT);
	} else {
		modify_event(epollfd, fd, EPOLLIN);
	}
	memset(buf, 0, MAXSIZE);
}
