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

#include        <string.h>

#include	"chat.h"

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

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  tcp_read
 *  Description:  Read characters from tcp socket.
 * =====================================================================================
 */
    void
tcp_read ( struct chat_client *chat_client_p ){
    char buffer[256];
    char message[320];
    int retval;
    int nsfd;
    char *hello = "Bitte gib deinen Nickname ein: "; 
    int len;
    int message_terminated;

    nsfd = chat_client_p->socket;

    // Tell client that he has to enter his nickname first
    send(nsfd, hello, strlen(hello), 0);

    for(;;){
        // read message from socket
        retval = read(nsfd, buffer, sizeof(buffer),0);
        
        //if a client disconnectes, we leave the function and the threads ends
        if(retval == 0){
            close_connection(chat_client_p);
            return;
        } 

        //TODO: Error handling
        //TODO: Use strncpy and strncat instead of strcpy, strcat, or even better memcpy/memcmp?
        if (retval != -1){
            if(strlen(chat_client_p->nickname)==0){
                len = strlen(buffer);
                if(buffer[len-1] == '\n')
                    len=len-2;
                strncpy(chat_client_p->nickname, buffer, len);
                strcpy(message, chat_client_p->nickname);
                strcat(message, " joined chat\n");
                write_message( chat_client_p, message);
            }
            else{
                // disconnect if clients sends /quit command
                // TODO: Why does telnet send \r\n?
                char command[8] = "/quit\r\n\0";
                if(strcmp(buffer, command) == 0){
                    close_connection(chat_client_p);
                    return;
                }

                // write nickname, if the last message was terminated by \n
                if(message_terminated){
                    strcpy(message, chat_client_p->nickname);
                    strcat(message, ": ");
                    strcat(message, buffer);
                }
                else{
                    strcpy(message, buffer);
                }

                write_message( chat_client_p, message);
            }

            // check if the message is \n terminated
            len = strlen(message); 
            if(message[len-1] == '\n'){
                message_terminated = 1; 
            }
            else{
                message_terminated = 0; 
            }

            // empty buffer
            memset(&buffer[0], 0, sizeof(buffer));
            memset(&message[0], 0, sizeof(message));
        }
    }
}				/* ----------  end of function tcp_read ---------- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  write_line
 *  Description:  Write a given line to all clients.
 * =====================================================================================
 */
//TODO: Diese Funktion wird aus jedem Thread aufgerufen, ist es ok, wenn ich beim write jeden Socket mit einer Mutex schütze? Die Funktion wäre dann parallel aufrufbar, oder?
    int 
write_message ( struct chat_client *chat_client_p, char message[320] )
{
    list_node_t *cur;

    for(cur=chat_client_p->ll->first; cur != NULL; cur=cur->next_p){
        pthread_mutex_lock(&(cur->mutex));

        int *cur_socket = cur->data_p;
        int *socket = cur->data_p;
        
        // do not send message to myself
        if(*socket != chat_client_p->socket){
            send(*socket, message, strlen(message), 0);
        }
        pthread_mutex_unlock(&(cur->mutex));
    }

    return EXIT_SUCCESS;
}		/* -----  end of function write_line  ----- */
