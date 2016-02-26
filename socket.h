#ifndef SOCKET_H_
#define SOCKET_H_

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <iosfwd>
#include <iostream>
#include <list>

using namespace std;

#define LENGTH 1024
#define LISTENNUM 5

class Socket{
public:
	Socket();
	Socket(int domain, int type, int protocol, int port, char* ip, char* buf);
	~Socket();
	int getFd() const { return fd_; }
	int getDomain() const { return domain_; }
	int getPort() const { return port_; }
	int getType() const { return type_; }
	int getProtocol() const { return protocol_; }
	char* getIp() const { return ip_; }
	void setDomain(int domain) { domain_ = domain; }
	void setType(int type) { type_ = type; }
	void setProtocol(int protocol) { protocol_ = protocol; }
	void setPort(int port) { port_ = port; }
	void setIp(char* ip) { ip_ = ip; }
	void init(int domain, int type, int protocol, int port, char* ip, char* buf);
	int socketCreate();
	int socketBind() const;
	int socketListen() const;
	int socketConnect() const;
	int socketAccept();
	int socketClose() const;
	void cleanBuf();
	int getClientFd() const { return clientfd_; }
	friend ostream& operator<<(ostream&, const Socket&);
private:
	void init();
	int fd_;
	int clientfd_;
	int domain_;
	int type_;
	int protocol_;
	int port_;
	char* ip_;
	char* buf_;
	struct sockaddr_in servaddr;
	list<int> clients_;
};

#endif
