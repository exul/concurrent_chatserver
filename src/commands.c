/*
 * =====================================================================================
 *
 *       Filename:  commands.c
 *
 *    Description:  Parse and execute commands sent by client.
 *
 *        Version:  1.0
 *        Created:  06/08/2012 11:14:12 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Andreas Brönnimann
 *   Organization:  v-net.ch
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <string.h>

#include "chat.h"

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
}               /* -----  end of function handle_command  ----- */

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
}               /* -----  end of function set_nickname  ----- */
