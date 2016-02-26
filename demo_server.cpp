#include <iostream>
#include <list>

#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#include "socket.h"
#include "common.h"
#include "epoll.h"

using namespace std;

#define IP "127.0.0.1"
#define PORT 9900
#define LENGTH 1024
// #define EPOLLEVENTS 100
#define FDSIZE 1024

static void do_epoll(Socket& socket);

// static void handle_events(int epollfd, struct epoll_event* events, int num, Socket& socket, char* buf);

// static void handle_accept(int epollfd, Socket& socket);

// static void do_read(int epollfd, int fd, char* buf);

// static void do_write(int epollfd, int fd, char* buf);

int main(int argc, char* argv[]){
	Socket s;
	char buf[LENGTH];
	s.init(AF_INET, SOCK_STREAM, 0, PORT, IP, buf); 
	cout<<s<<endl;
	if(s.socketCreate() == -1){
		exit(1);
	}

	if(s.socketBind() == -1){
		goto exit;
	}

	if(s.socketListen() == -1){
		goto exit;
	}
	
	do_epoll(s);
exit:
	s.socketClose();
	exit(1);
	return 0;
}

static void do_epoll(Socket& socket){
	Epoll epoll(FDSIZE);
	epoll.add_event(socket.getFd(), EPOLLIN);
	for(;;){
		epoll.epollWait(socket);
	}
}