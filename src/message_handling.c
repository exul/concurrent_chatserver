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
//TODO: Check if we allocated the memory correctly (struct pointers, and pointers inside structs)
    void
tcp_read ( struct chat_client *chat_client_p ){
    char buffer[MAX_BUFFER_LEN];
    char message[MAX_MESSAGE_LEN];
    int retval;
    int nsfd;
    char *hello = "Bitte gib deinen Nickname ein: ";
    char *nickl = "Sorry, dein Nickname ist zu lang, Nickname wurde gekürzt!\n";
    int len;
    int message_terminated;

    nsfd = chat_client_p->socket;

    // Tell client that he has to enter his nickname first
    send(nsfd, hello, strlen(hello), 0);

    for(;;){
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
                memset(&chat_client_p->nickname[0], 0, sizeof(chat_client_p->nickname));

                len = strlen(buffer);
                if(buffer[len-1] == '\n')
                    len=len-2;
                if(len > MAX_NICK_LEN){
                    len = MAX_NICK_LEN;
                    send(nsfd, nickl, strlen(nickl), 0);
                }

                strncpy(chat_client_p->nickname, buffer, len);
                strncpy(message, chat_client_p->nickname, MAX_BUFFER_LEN);
                strncat(message, " joined chat\n", MAX_BUFFER_LEN);
                write_message( chat_client_p, message);
            }
            else{
                // disconnect if clients sends /quit command
                // telnet sends \r\n
                char command[8] = "/quit\r\n\0";
                if(strcmp(buffer, command) == 0){
                    close_connection(chat_client_p);
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

            // empty buffer
            memset(&buffer[0], 0, sizeof(buffer));
            memset(&message[0], 0, sizeof(message));
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
write_message ( struct chat_client *chat_client_p, char message[320] )
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
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  handle_command
 *  Description:  Check if message contains a command and handles it.
 * =====================================================================================
 */
    int
handle_command ( struct chat_client *chat_client_p, char message[320] )
{
    // command found and executed
    return 0;
}		/* -----  end of function handle_command  ----- */
