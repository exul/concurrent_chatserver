/*
 * =====================================================================================
 *
 *       Filename:  chat.c
 *
 *    Description:  A primitive chat system.
 *
 *        Version:  1.0
 *        Created:  03/30/2012 09:48:38 AM
 *       Revision:  {node|short}
 *       Compiler:  gcc
 *
 *         Author:  Andreas Brönnimann (v-net.ch), 
 *        Company:  v-net.ch
 *
 * =====================================================================================
 */

#include	<stdlib.h>
#include	<stdio.h>

#include        <sys/types.h>
#include        <sys/socket.h>
#include        <netdb.h>

#include        <pthread.h>

#include	"chat.h"
#include	"server_conn_handling.h"

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  Obviously the main function to start the chat.
 * =====================================================================================
 */
    int
main ( int argc, char *argv[] )
{
    extern int optind;

    char *port;
    char *address;
    int sfd;

    int nsfd;

    //allow max 256 theads
    pthread_t threads[256];

    if (argc < 3) {
        fprintf(stderr, "Not enough parameters to start!\n");
        exit(EXIT_FAILURE);
    }

    address = argv[optind++];
    port = argv[optind++];

    //create new socket and listen on it
    sfd = server_listen(address, port);

    //wait for new connection and create a thread when we get one
    for(;;){
        void *arg;
        nsfd = new_connection(sfd);

        printf("Socket: %i\n", nsfd);
        pthread_create(&threads[nsfd], NULL, (void*)tcp_read, (void*) &nsfd);
    }

    close(nsfd);

    close(sfd);
        
    return EXIT_SUCCESS;
}				/* ----------  end of function main  ---------- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  tcp_read
 *  Description:  Read characters from tcp socket.
 * =====================================================================================
 */
    void
tcp_read ( int *arg ){
    char buffer[256];
    int bytes_recv;
    int nsfd;

    nsfd = *arg;

    for(;;){
        // read message from socket
        bytes_recv = recv(nsfd, buffer, sizeof(buffer),0);

        if (bytes_recv > 0){
            printf("Client sent: %s", buffer); 
        }
    }
}				/* ----------  end of function tcp_read ---------- */
