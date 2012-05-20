/*
 * =====================================================================================
 *
 *       Filename:  linked_list.c
 *
 *    Description:  A thread save linked list.
 *
 *        Version:  1.0
 *        Created:  05/19/2012 11:48:20 AM
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
#include "linked_list.h"


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  linked_list_init
 *  Description:  Initialize linked list
 * =====================================================================================
 */
   int 
linked_list_init ( linked_list_t *linked_list_p )
{
    // we have no elements when we start
    linked_list_p->first = NULL;
    linked_list_p->last= NULL;
    linked_list_p->next_index = 0;
    // initialize mutex
    int retval = pthread_mutex_init(&(linked_list_p->mutex), NULL);
    // check, if initializing was successful
    if (retval != 0)
        fprintf(stderr, "pthread_mutex_init error %d",retval), exit(1);
    return 0;
}		/* -----  end of function linked_list_init  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  linked_list_insert
 *  Description:  Insert new element into linked list.
 * =====================================================================================
 */
    int
linked_list_insert ( void **data_pp, linked_list_t *linked_list_p )
{
    list_node_t *new;

    // lock list
    pthread_mutex_lock(&(linked_list_p->mutex));
    // allocate memory for new node
    new = (list_node_t *)malloc(sizeof(list_node_t));
    // next index;
    new->index = linked_list_p->next_index++;
    // set data pointer
    new->data_p = data_pp;
    // set next_p to NULL, otherwise there just rubish in next_p and we get as segfault
    new->next_p = NULL;

    if(linked_list_p->first == NULL){
        linked_list_p->first = new; 
        linked_list_p->last = new; 
    }
    else{
        linked_list_p->last->next_p = new;
        linked_list_p->last = new; 
    }

    // unlock linked list
    pthread_mutex_unlock(&(linked_list_p->mutex));

    return 0;
}		/* -----  end of function linked_list_insert  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  linked_list_remove
 *  Description:  Remove element from linked list.
 * =====================================================================================
 */
    int
linked_list_remove ( int index, void **data_pp, linked_list_t *linked_list_p )
{
    list_node_t *cur, *prev;

    // lock linked list
    pthread_mutex_lock(&(linked_list_p->mutex));

    // loop through list
    for(cur=prev=linked_list_p->first; cur != NULL; prev=cur, cur=cur->next_p){
        // we found the element
        if(cur->index == index){
            *data_pp = cur->data_p; 
            prev->next_p = cur->next_p; 
            free(cur);
            break;
        } 
        else if (cur->index > index){
            break; 
        }
    }

    // unlock linked list
    pthread_mutex_unlock(&(linked_list_p->mutex));

    return 0;
}		/* -----  end of function linked_list_remove  ----- */
