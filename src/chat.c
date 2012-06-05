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

#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <pthread.h>

#include <unistd.h>

#include <string.h>

#include "chat.h"

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  Obviously the main function to start the chat.
 * =====================================================================================
 */
//TODO: Check if we allocated the memory correctly (struct pointers, and pointers inside structs)
    int
main ( int argc, char *argv[] )
{
    extern int optind;

    char *port;
    char *address;
    int sfd;

    linked_list_t ll;

    //allow max 256 theads
    pthread_t threads[256];

    if (argc < 3) {
        fprintf(stderr, "Not enough parameters to start!\n");
        printf("Usage: chat address port (e.g. chat localhost 51234)\n");
        exit(EXIT_FAILURE);
    }

    address = argv[optind++];
    port = argv[optind++];

    //create new socket and listen on it
    sfd = server_listen(address, port);

    //initialize linked list
    linked_list_init(&ll);

    //wait for new connection and create a thread when we get one
    for(;;){
        struct chat_client *new_client_p;
        int nsfd;
        // get socket file desciptor for new client
        nsfd = new_connection(sfd);

        // create struct to store client information
        new_client_p = (struct chat_client *)malloc(sizeof(struct chat_client));
        new_client_p->socket = nsfd;
        new_client_p->ll = &ll;
        strcpy(new_client_p->nickname, "");

        // create a thread for each client
        pthread_create(&threads[nsfd], NULL, (void*)tcp_read, (void*)new_client_p);

        // add socket file descriptor to linked list
        // TODO: Isn't realy nice => We may wan't to add clients to the list instead of sockets
        linked_list_insert((void*)&(new_client_p->socket), &ll);
        // TODO: This should be done nicer. Do we really need this information here?
        new_client_p->index = new_client_p->ll->last->index;
    }

    close(sfd);
        
    return EXIT_SUCCESS;
}				/* ----------  end of function main  ---------- */
