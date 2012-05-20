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

//TODO: linked_list.h has to be included before chat.h, that's not nice
#include        "linked_list.h"
#include	"chat.h"
#include	"server_conn_handling.h"

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
        int *nsfd_p;
        //nsfd_p = (int *)malloc(sizeof(int));
        nsfd = new_connection(sfd);
        //nsfd_p = &nsfd;

        //printf("Pointer to nsfd is %p\n", &nsfd);
        //printf("Pointer to nsfd is %p\n", nsfd_p);

        // add socket file descriptor to linked list
        //linked_list_insert((void*)&nsfd, &ll);

        printf("Socket: %i\n", nsfd);
        // create struct to store client information
        new_client_p = (struct chat_client *)malloc(sizeof(struct chat_client));
        new_client_p->socket = nsfd;
        printf("Socket in struct is %i\n", new_client_p->socket);
        new_client_p->ll = &ll;
        // TODO: Ask user for nickname, check if it's longer than 50 characters
        // create a thread for each client
        pthread_create(&threads[nsfd], NULL, (void*)tcp_read, (void*)new_client_p);

        // add socket file descriptor to linked list
        // TODO: Isn't realy nice => We may wan't to add clients to the list instead of sockets
        linked_list_insert((void*)&(new_client_p->socket), &ll);

        //TODO: Manage disconnecting clients
    }

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
tcp_read ( struct chat_client *chat_client_p ){
    char buffer[256];
    int bytes_recv;
    int nsfd;

    list_node_t *cur;

    //nsfd = *arg;
    nsfd = chat_client_p->socket;
    printf("Socket in struct in function is %i\n", nsfd);

    for(;;){
        // read message from socket
        // TODO: Some of the old bytes are repeated in the new message, why?
        // We need a better end of line detection!
        bytes_recv = recv(nsfd, buffer, sizeof(buffer),0);

        if (bytes_recv > 0){
            printf("Client sent: %s", buffer); 

            //TODO: Use a lock for every entry, no global lock!
            pthread_mutex_lock(&(chat_client_p->ll->mutex));
            //printf("The pointer to the first entry is: %i", chat_client_p->ll->first->next_p->index);
            for(cur=chat_client_p->ll->first; cur != NULL; cur=cur->next_p){
                int *socket;
                socket = cur->data_p;
                printf("Client with index %i, Socket-FD: %i\n", cur->index, *socket);
                //printf("Client with index %i, Socket-FD: %p\n", cur->index, cur->data_p);
            }
            pthread_mutex_unlock(&(chat_client_p->ll->mutex));
        }
    }
}				/* ----------  end of function tcp_read ---------- */
