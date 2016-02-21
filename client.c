/*
** client.c -- a stream socket client demo

Steps to establish a socket on client side:
1. socket()
2. connect()
3. read() and write()


*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <png.h>

#include <arpa/inet.h>

#define PORT "3490" // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once 

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
		return &(((struct sockaddr_in*)sa)->sin_addr);

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
	int sockfd; // the socket file descriptor
	int numbytes;
	char buf[MAXDATASIZE]; // char arr to store received message
	struct addrinfo hints;
	struct addrinfo *servinfo;
	struct addrinfo *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	if (argc != 2) {
	    fprintf(stderr,"usage: client hostname\n");
	    exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	/*=== argv[1]: localhost, PORT:"3490" ===*/
	if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) 
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	/*=== loop through all the results and connect to the first we can ===*/
	int counter = 1;
	for(p = servinfo; p != NULL; p = p->ai_next) 
	{
		printf("loop: %d\n",counter); counter++;
		/*==== socket system call ====*/
		if ( ( sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol) ) == -1 ) 
		{
			perror("client: socket");
			continue;
		}

		/*======= connect to host ======*/
		if ( connect(sockfd, p->ai_addr, p->ai_addrlen) == -1 ) 
		{
			perror("client: connect");
			close(sockfd);
			continue;
		}

		break;
	}

	/*====== if server is not available ======*/
	if (p == NULL) 
	{
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}
	/*===== get server IP address, and assign it to s ======*/
	inet_ntop(p->ai_family, get_in_addr( (struct sockaddr *)p->ai_addr ),
			s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure

	/*==== receive message from server ====*/
	if ( ( numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0) ) == -1 ) 
	{
	    perror("recv");
	    exit(1);
	}

	buf[numbytes] = '\0'; // set the end of string

	printf("client: received '%s'\n",buf);

	close(sockfd);

	return 0;
}

