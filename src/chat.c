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

    if (argc < 3) {
        fprintf(stderr, "Not enough parameters to start!\n");
        exit(EXIT_FAILURE);
    }

    address = argv[optind++];
    port = argv[optind++];

    //create new socket and listen on it
    sfd = server_listen(address, port);

    //wait for new connection and create a socket when we get one
    nsfd = new_connection(sfd);

    close(nsfd);

    close(sfd);
        
    return EXIT_SUCCESS;
}				/* ----------  end of function main  ---------- */
