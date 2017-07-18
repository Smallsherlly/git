#ifndef STRUCT_H_
#define STRUCT_H_

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

typedef struct sockaddr* SP;
typedef struct sockaddr_in SA;

typedef struct user_info
{
	char user_name[10][20];
	SA 	 user_addr[10];
}user;

typedef struct msg_pub
{
	int msg_num;
	char msg[100][255];
}MSG;






#endif  //STRUCT_H_
