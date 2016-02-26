#ifndef EPOLL_H_
#define EPOLL_H_

#include <sys/epoll.h>
#include "socket.h"

#define EPOLLEVENTS 1024

class Epoll{
public:
	Epoll(int s);
	~Epoll();
	int getEpollFd() const { return epollfd; }
	void epollWait(Socket& socket); 
	void add_event(int fd, int state) const;
	
private:
	
	void delete_event(int fd, int state) const;
	void modify_event(int fd, int state) const;
	void handle_events(Socket& socket, char* buf);
	void do_accept(Socket& socket);
	void do_read(int fd, char* buf);
	void do_write(int fd, char* buf);
	struct epoll_event events[EPOLLEVENTS];
	int num;
	int size;
	int epollfd;
};

#endif
