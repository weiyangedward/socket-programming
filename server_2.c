/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>


#define FILE_SIZE (100)

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, clilen;
     char buffer[FILE_SIZE]; // buffer to hold message received
     struct sockaddr_in serv_addr, cli_addr; // struct to hold internet address
     int n; // number of chars read() and write()

     if (argc < 3) // if input invalid
     {
         fprintf(stderr,"usage: server_2 port text_file\n");
         exit(1);
     }

     /*==== read from a file =====*/
    FILE *fp;
    long lSize;
    char *content;

    fp = fopen ( argv[2], "r" );
    if( !fp ) perror(argv[2]),exit(1);

    fseek( fp , 0L , SEEK_END);
    lSize = ftell( fp );
    rewind( fp );

    /* allocate memory for entire content */
    content = calloc( 1, lSize+1 );
    if( !content ) fclose(fp),fputs("memory alloc fails",stderr),exit(1);

    /* copy the file into the buffer */
    if( 1!=fread( content , lSize, 1 , fp) )
      fclose(fp),free(content),fputs("entire read fails",stderr),exit(1);

    /* do your work here, buffer is a string contains the whole text */

    fclose(fp);
    

     /*=== creates a new socket: socket(address_domain, socket_type, protocol), 
     returns an entry (sockfd) in the file descriptor table =================*/
     sockfd = socket(AF_INET, SOCK_STREAM, 0); 
     if (sockfd < 0) // return -1 if fails
        error("ERROR opening socket");

    /*===== assign values to serv_addr ======*/
     bzero((char *) &serv_addr, sizeof(serv_addr)); // initialize serv_addr to zeros

     serv_addr.sin_family = AF_INET; // code for address family, set to AF_INET
     serv_addr.sin_addr.s_addr = INADDR_ANY; // IP address of the host, use INADDR_ANY to get this address

     portno = atoi(argv[1]); // convert port number from str to int
     serv_addr.sin_port = htons(portno); // port number, convert port number from 'host_byte_order' to 'network_byte_order'

     /*=== bind a socket to an address: bind(socket_file_descriptor, address, size_of_address) ===*/
     if ( bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0 ) 
              error("ERROR on binding");
    /*=== listen on the socket for connections: listen(socket_file_descriptor, number_of_connections_waiting(max=5)) ===*/
     listen(sockfd,5);

     clilen = sizeof(cli_addr); // size of address of client
     /*=========================
    accept() blocks the process until a client connects to the server, and returns a new file decriptor
    accept(socket_file_descriptor, client_address, address_size)
     */
     newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     if (newsockfd < 0) 
          error("ERROR on accept");

     bzero(buffer,FILE_SIZE); // initialize buffer to zeros
     // n = read(newsockfd,buffer,FILE_SIZE); // read from socket
     // if (n < 0)  // if nothing has been read
     //    error("ERROR reading from socket");

     // printf("Here is the message: %s\n",buffer); // print out buffer
     n = write(newsockfd,content,lSize); // write a message to client
     if (n < 0) 
        error("ERROR writing to socket");
    free(content);
    close(newsockfd);
    close(sockfd);

     return 0; 
}