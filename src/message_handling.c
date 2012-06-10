/*
 * =====================================================================================
 *
 *       Filename:  message_handling.c
 *
 *    Description:  Read and write messages.
 *
 *        Version:  1.0
 *        Created:  06/03/2012 09:16:42 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Andreas Brönnimann (v-net.ch)
 *   Organization:  v-net.ch
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <pthread.h>

#include <string.h>

#include <unistd.h>

#include "chat.h"

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  tcp_read
 *  Description:  Read characters from tcp socket.
 * =====================================================================================
 */
    void
tcp_read ( struct chat_client *chat_client_p ){
    char buffer[MAX_BUFFER_LEN];
    char message[MAX_MESSAGE_LEN];
    int retval;
    int retval_command;
    int nsfd;
    char *hello = "Bitte gib deinen Nickname ein: ";
    int len;
    int message_terminated;

    nsfd = chat_client_p->socket;

    // Tell client that he has to enter his nickname first
    send(nsfd, hello, strlen(hello), 0);

    for(;;){
        // clean memory
        memset(&buffer[0], 0, sizeof(buffer));
        memset(&message[0], 0, sizeof(message));

        // read message from socket
        retval = read(nsfd, buffer, sizeof(buffer));

        //if a client disconnectes, we leave the function and the threads ends
        if(retval == 0){
            close_connection(chat_client_p);
            return;
        }

        // Use strnpy and strncat, since we don't send binary data that's ok
        if (retval != -1){
            if(strlen(chat_client_p->nickname)==0){
                set_nickname(chat_client_p, buffer);

                strncpy(message, chat_client_p->nickname, MAX_BUFFER_LEN);
                strncat(message, " joined chat\n", MAX_BUFFER_LEN);
                write_message( chat_client_p, message);
            }
            else{
                retval_command = handle_command(chat_client_p, buffer);

                // command found and handled
                if(retval_command == 0){
                    continue;
                }
                // command found, but we want to exit this function
                else if(retval_command == -1){
                    return; 
                }
                
                // write nickname, if the last message was terminated by \n
                if(message_terminated){
                    strncpy(message, chat_client_p->nickname, MAX_NICK_LEN);
                    strncat(message, ": ", 2);
                    strncat(message, buffer, MAX_BUFFER_LEN);
                }
                else{
                    strncpy(message, buffer, MAX_BUFFER_LEN);
                }

                write_message(chat_client_p, message);
            }

            // check if the message is \n terminated
            len = strlen(message);
            if(message[len-1] == '\n'){
                message_terminated = 1;
            }
            else{
                message_terminated = 0;
            }
        }
    }
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  write_line
 *  Description:  Write a given line to all clients.
 * =====================================================================================
 */
    void
write_message ( struct chat_client *chat_client_p, char message[MAX_MESSAGE_LEN] )
{
    list_node_t *cur;

    for(cur=chat_client_p->ll->first_p; cur != NULL; cur=cur->next_p){
        pthread_mutex_lock(&(cur->mutex));

        int *socket = cur->data_p;

        // do not send message to myself
        if(*socket != chat_client_p->socket){
            send(*socket, message, strlen(message), 0);
        }
        pthread_mutex_unlock(&(cur->mutex));
    }
}       /*  -----  end of function write_message ----- */
