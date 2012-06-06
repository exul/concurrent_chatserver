/*
 * =====================================================================================
 *
 *       Filename:  linked_list.h
 *
 *    Description:  Include file for linked list.
 *
 *        Version:  1.0
 *        Created:  05/19/2012 11:55:35 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Andreas Brönnimann (v-net.ch)
 *   Organization:  v-net.ch
 *
 * =====================================================================================
 */

#include <pthread.h>

#define MAX_BUFFER_LEN 254
#define MAX_NICK_LEN 64
#define MAX_MESSAGE_LEN 320

typedef struct list_node {
    int index;
    void *data_p;
    struct list_node *next_p;
    pthread_mutex_t mutex;
} list_node_t;

typedef struct linked_list {
    int next_index;
    list_node_t *first_p;
    list_node_t *last_p;
    pthread_mutex_t mutex;
} linked_list_t;

int linked_list_init( linked_list_t *linked_list_p );
int linked_list_insert( void *data_pp, linked_list_t *linked_list_p );
int linked_list_remove( int index, linked_list_t *linked_list_p );

struct chat_client {
   int socket; 
   struct linked_list *ll;
   char nickname[MAX_NICK_LEN];
   int index;
};

int new_connection (int sfd);

int server_listen (char *address, char *port);
int new_connection (int sfd);
int close_connection (struct chat_client *chat_client_p);

void tcp_read (struct chat_client *chat_client_p);
void write_message (struct chat_client *chat_client_p, char message[MAX_MESSAGE_LEN]);
int handle_command (struct chat_client *chat_client_p, char buffer[MAX_BUFFER_LEN]);
char* left (char* str, int len);
