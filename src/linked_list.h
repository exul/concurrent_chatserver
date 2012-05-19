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

typedef struct list_node {
    int index;
    void *data_p;
    struct list_node *next_p;
} list_node_t;

typedef struct linked_list {
    list_node_t *first;
    pthread_mutex_t mutex;
} linked_list_t;

int linked_list_init( linked_list_t *linked_list_p );
int linked_list_insert( int index, void **data_pp, linked_list_t *linked_list_p );
int linked_list_remove( int index, void **data_pp, linked_list_t *linked_list_p );
