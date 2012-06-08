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
//TODO: Maybe we don't need stdio
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
//TODO: Check if we allocated the memory correctly (struct pointers, and pointers inside structs)
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
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  handle_command
 *  Description:  Check if message contains a command and handles it.
 * =====================================================================================
 */
    int
handle_command ( struct chat_client *chat_client_p, char buffer[MAX_BUFFER_LEN] )
{
    char message[MAX_MESSAGE_LEN];
    char *known = " is now known as ";
    char *nickname_old = (char *)malloc(strlen(chat_client_p->nickname) * sizeof(char));
    // disconnect if clients sends /quit command
    if(strcmp(substr(buffer,0,5), "/quit") == 0){
        close_connection(chat_client_p);
        return -1;
    }

    // change nickname if client sends /nick
    if(strcmp(substr(buffer,0,5), "/nick") == 0){
        char *nickname = substr(buffer, 6, strlen(buffer));

        // save old nickname
        strncpy(nickname_old, chat_client_p->nickname, strlen(chat_client_p->nickname));

        set_nickname(chat_client_p, nickname);
        //TODO: Just for debugging
        printf("Change nickname: %s\n", substr(buffer, 6, strlen(buffer)));

        // write message that the user has a new nickname
        strncpy(message, nickname_old, strlen(nickname_old));
        strncat(message, known, strlen(known));
        strncat(message, chat_client_p->nickname, strlen(chat_client_p->nickname));
        strncat(message, "\n", 1);
        write_message( chat_client_p, message);
        return 0;
    }

    memset(&message[0], 0, sizeof(message));

    // command found and executed
    return 1;
}		/* -----  end of function handle_command  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  substr
 *  Description:  Get substring from char array.
 * =====================================================================================
 */
    char*
substr ( char* str, int offset,int len )
{
    char *command = (char *)malloc(len * sizeof(char));
    strncpy(command, str+offset, len);
    return command;
}		/* -----  end of function substr  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  set_nickname
 *  Description:  
 * =====================================================================================
 */
    void
set_nickname ( struct chat_client *chat_client_p, char buffer[MAX_BUFFER_LEN] )
{
    char *nickl = "Sorry, dein Nickname ist zu lang, Nickname wurde gekürzt!\n";
    int len = strlen(buffer);
    int nsfd = chat_client_p->socket;

    //clean memory
    memset(&chat_client_p->nickname[0], 0, sizeof(chat_client_p->nickname));

    // remove newline
    if(buffer[len-1] == '\n')
        len=len-2;
    if(len > MAX_NICK_LEN){
        len = MAX_NICK_LEN;
        send(nsfd, nickl, strlen(nickl), 0);
    }

    strncpy(chat_client_p->nickname, buffer, len);

    return ;
}		/* -----  end of function set_nickname  ----- */
