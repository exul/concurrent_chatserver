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

#include        <unistd.h> 
#include        <stdio.h>

#include        <err.h>

#include        <fcntl.h>

#include        <string.h>

#include        "chat.h"

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
    int sfd, s;
    struct protoent *pe;

    pe = getprotobyname("tcp");

    hints.ai_protocol = pe->p_proto;
    hints.ai_family = AF_UNSPEC; /* allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* only stream sockes */
    hints.ai_flags = AI_CANONNAME;
    hints.ai_addrlen = 0;
    hints.ai_addr = 0;
    hints.ai_canonname = 0;

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

        /* avoid TIME_WAIT problem */
        int on = 1;
        int status = setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (const char *) &on, sizeof(on));

        if (status == -1)
        {
            perror("setsockopt(...,SO_REUSEADDR,...)");
        }

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

    nsfd = accept(sfd, (struct sockaddr *)&addr, &len);
    if (nsfd == -1) {
        err(1, "accept");
        exit(EXIT_FAILURE);
    }

    return nsfd;
}		/* -----  end of function new_connection  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  client_disconnect
 *  Description:  Disconnects a client from the server
 * =====================================================================================
 */
    int 
close_connection( struct chat_client *chat_client_p )
{
    char message[320];

    strcpy(message, chat_client_p->nickname);
    strcat(message, " disconnected\n");
    write_message(chat_client_p, message);

    // remove socket from linked list
    linked_list_remove(chat_client_p->index, chat_client_p->ll);

    close(chat_client_p->socket);

    free(chat_client_p);
    return EXIT_SUCCESS;
}
