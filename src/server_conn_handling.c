/*
 * =====================================================================================
 *
 *       Filename:  server_conn_handling.c
 *
 *    Description:  A simple TCP server
 *
 *        Version:  1.0
 *        Created:  03/31/2012 12:14:47 AM
 *       Revision:  {node|short}
 *       Compiler:  gcc
 *
 *         Author:  Andreas Brönnimann (v-net.ch), 
 *        Company:  v-net.ch
 *
 * =====================================================================================
 */

#include        <stdlib.h>
#include        <stdio.h>

#include        <sys/types.h>
#include        <sys/socket.h>
#include        <netdb.h>

#include        <fcntl.h>

static const int MAX_CONST_PENDING = 10;

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  server_listen
 *  Description:  Create new socket, bind it to an address and begin listening.
 * =====================================================================================
 */
    int 
server_listen ( char *address, char *port )
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, s, flags;

    hints.ai_family = AF_UNSPEC; /* allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* only stream sockes */
    hints.ai_flags = AI_PASSIVE; /* For wildcard IP address */

    s = getaddrinfo(address, port, &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }   

    /* getaddrinfo() returns a list of address structures.
      Try each address until we successfully bind(2).
      If socket(2) (or bind(2)) fails, we (close the socket
      and) try the next address. 
    */

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype,
            rp->ai_protocol);

        if (sfd == -1) 
            continue;

        /* Success */
        if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
            break;    

        close(sfd);
    }   

    /* No address succeeded */
    if (rp == NULL) {    
        fprintf(stderr, "Could not bind socket to port.\n");
        exit(EXIT_FAILURE);
    }   

    freeaddrinfo(result);

    /* start listening */
    if(listen(sfd, MAX_CONST_PENDING) == -1){
        fprintf(stderr, "Can't listen on socket.");
        exit(EXIT_FAILURE);
    }
    return sfd;
}		/* -----  end of function server_listen  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  new_connection
 *  Description:  Create a socket for new clients.
 * =====================================================================================
 */
    int 
new_connection (int sfd)
{
    struct sockaddr_storage addr;
    socklen_t len = sizeof(addr);
    int nsfd;
    int flags;

    nsfd = accept(sfd, (struct sockaddr *)&addr, &len);
    if (nsfd == -1) {
        err(1, "accept");
        exit(EXIT_FAILURE);
    }

    /* set socket to nonblocking */
    flags = fcntl(nsfd, F_GETFL);

    if (fcntl(nsfd, F_SETFL, flags | O_NONBLOCK) < 0){
        printf("Unable to set socket to nonblocking."); 
        exit(EXIT_FAILURE);
    }

    printf("New client connected\n");

    return nsfd;
}		/* -----  end of function new_connection  ----- */
