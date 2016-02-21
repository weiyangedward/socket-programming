#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>  // for hostent
#include <stdlib.h>
#include <string.h>

#define FILE_SIZE (100)

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;

    struct sockaddr_in serv_addr;
    struct hostent *server; // host computer pointer server

    char buffer[FILE_SIZE];

    if (argc < 3) 
    {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }

    
    /*======= creates a new socket ========*/
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        error("ERROR opening socket");
    }

    server = gethostbyname(argv[1]); // takes server name as input and return server info
    if (server == NULL) 
    {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr)); // initialize serv_addr to zeros

    serv_addr.sin_family = AF_INET; // code for address family, set to AF_INET
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length); // bcopy(char *s1, char *s2. s1_length): copy length bytes from s1 to s2

    portno = atoi(argv[2]); // port number
    serv_addr.sin_port = htons(portno);

    /*===== establish a connection to server =========*/
    if (connect( sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr) ) < 0)
    {
        error("ERROR connecting");
    }

    printf("client: connecting to %s\n", server->h_name);
    /*===== get input from std stream ======*/
    // printf("Please enter the message: ");
    // bzero(buffer,FILE_SIZE);
    // fgets(buffer,FILE_SIZE,stdin);

    /*==== write message to server ======*/
    // n = write(sockfd,buffer,strlen(buffer));
    // if (n < 0) 
    //      error("ERROR writing to socket");

    /*==== read message from server =======*/
    bzero(buffer,FILE_SIZE);
    n = read(sockfd,buffer,FILE_SIZE);
    if (n < 0) 
    {
        error("ERROR reading from socket");
    }

    printf("client: received %d bytes\n\n",n);

    printf("%s\n",buffer);

    close(sockfd);
    return 0;
}