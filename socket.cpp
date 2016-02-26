#include "socket.h"
#include <string.h>

Socket::Socket(){

}

Socket::Socket(int domain, int type, int protocol, int port, char* ip, char* buf){
	domain_ = domain;
	type_ = type;
	protocol_ = protocol;
	port_ = port;
	ip_ = ip;
	buf_ = buf;
}

Socket::~Socket(){
}

void Socket::init(int domain, int type, int protocol, int port, char* ip, char* buf){
	domain_ = domain;
	type_ = type;
	protocol_ = protocol;
	port_ = port;
	ip_ = ip;
	buf_ = buf;
	init();
}


void Socket::init(){
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = domain_;
	servaddr.sin_port = htons(port_);
	inet_pton(domain_, ip_, &servaddr.sin_addr);
}

int Socket::socketCreate(){
	fd_ = socket(domain_, type_, protocol_);
	if(fd_ == -1){
		fprintf(stderr, "create socket fail!\n");
		return -1;
	}
	return 0;
}

int Socket::socketBind() const{
	if(bind(fd_, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
		fprintf(stderr, "bind socket fail!\n");
		return -1;
	}
	return 0;
}

int Socket::socketListen()const{
	if(listen(fd_, LISTENNUM) == -1){
		fprintf(stderr, "listen socket fail!\n");
		return -1;
	}
	return 0;
}

int Socket::socketAccept(){
	struct sockaddr_in clientaddr;
	socklen_t clientaddrlen = sizeof(clientaddr);
	clientfd_ = accept(fd_, (struct sockaddr*)&clientaddr, &clientaddrlen);
	if(clientfd_ == -1){
		fprintf(stderr, "accept client socket fail!\n");
		return -1;
	}

	cout<<"Accept a new client:"<<inet_ntoa(clientaddr.sin_addr)<<':'<<clientaddr.sin_port<<endl;
//	clients_.push_back(clientfd_);
	return clientfd_;
}

int Socket::socketConnect() const{
	if(connect(fd_, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
		fprintf(stderr, "connect server fail!\n");
		return -1;
	}
	return 0;
}

int Socket::socketClose() const{
	if(close(fd_) == -1){
		fprintf(stderr, "close socket fail!\n");
		return -1;
	}
	return 0;
}

void Socket::cleanBuf(){
	memset(buf_, 0, sizeof(buf_) / sizeof(buf_[0]));
}


ostream& operator<<(ostream& os, const Socket& s){
	os<<'{'<<s.fd_<<','<<s.domain_<<',';
	os<<s.type_<<','<<s.protocol_;
	os<<s.ip_<<':'<<s.port_<<'}'<<endl;
	return os;
}
