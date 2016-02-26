#ifndef __COMMON_H
#define __COMMON_H

void add_event(int epollfd, int fd, int state);

void delete_event(int epollfd, int fd, int state);

void modify_event(int epollfd, int fd, int state);


#endif //end of __COMMON_H