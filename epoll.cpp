#include <iostream>

#include <string.h>

#include "epoll.h"

Epoll::Epoll(int s) : size (s) {
	epollfd = epoll_create(s);
	if(epollfd == -1){
		fprintf(stderr, "epoll create fail!\n");
		return;
	}
}

Epoll::~Epoll(){
}

void Epoll::epollWait(Socket& socket){
	char buf[EPOLLEVENTS];
	num = epoll_wait(epollfd, events, EPOLLEVENTS, -1);
	handle_events(socket, buf);
}

void Epoll::handle_events(Socket& socket, char* buf){
	int i, fd;
	for(i = 0; i < num; i++){
		fd = events[i].data.fd;
		if((fd == socket.getFd()) && (events[i].events & EPOLLIN)){
			do_accept(socket);
		} else if((events[i].events & EPOLLIN)){
			do_read(fd, buf);
		} else if((events[i].events & EPOLLOUT)){
			do_write(fd, buf);
		}
	}
}

void Epoll::do_accept(Socket& socket){
	int clientfd;
	if(socket.socketAccept() == -1){
		return;
	}
	clientfd = socket.getClientFd();
	add_event(clientfd, EPOLLIN);
}

void Epoll::do_read(int fd, char* buf){
	int nread;
	memset(buf, 0, size);
	nread = read(fd, buf, size);
	if(nread == 0){
		fprintf(stderr, "Client close!\n");
		close(fd);
		delete_event(fd, EPOLLIN);
	} else if(nread == -1){
		fprintf(stderr, "read fail!\n");
		close(fd);
		delete_event(fd, EPOLLIN);
	} else {
		cout<<"read message is:"<<buf;
		modify_event(fd, EPOLLOUT);
	}
}

void Epoll::do_write(int fd, char* buf){
	int nwrite;
	nwrite = write(fd, buf, strlen(buf));
	if(nwrite == -1){
		fprintf(stderr, "write fail!");
		close(nwrite);
		delete_event(fd, EPOLLOUT);
	} else {
		modify_event(fd, EPOLLIN);
	}

	memset(buf, 0, size);
}

void Epoll::add_event(int fd, int state) const{
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = state;
	int errno;
	if(epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == -1){
		fprintf(stderr, "add event fail!\n");
	}
}

void Epoll::delete_event(int fd, int state)const{
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = state;
	if(epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev) == -1){
		fprintf(stderr, "delete event fail!\n");
	}
}

void Epoll::modify_event(int fd, int state)const{
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = state;
	if(epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev) == -1){
		fprintf(stderr, "modify event fail!\n");
	}
}
