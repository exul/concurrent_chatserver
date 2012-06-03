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
#include "chat.h"


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
//TODO: Do cleanup, maybe first or last node isn't needed
//TODO: Do not use global lock
    int
linked_list_insert ( void *data_p, linked_list_t *linked_list_p )
{
    list_node_t *new;

    // lock list
    pthread_mutex_lock(&(linked_list_p->mutex));
    // allocate memory for new node
    new = (list_node_t *)malloc(sizeof(list_node_t));
    // next index;
    new->index = linked_list_p->next_index++;
    // set data pointer
    new->data_p = data_p;
    // set next_p to NULL, otherwise there just rubish in next_p and we get a segfault
    new->next_p = NULL;

    // initialize mutex
    int retval = pthread_mutex_init(&(new->mutex), NULL);
    // check, if initializing was successful
    if (retval != 0)
        fprintf(stderr, "pthread_mutex_init error %d",retval), exit(1);

    // set first/last node
    if(linked_list_p->first == NULL){
        linked_list_p->first = new; 
        linked_list_p->last = new; 
    }
    else{
        linked_list_p->last->next_p = new;
        linked_list_p->last = new; 
    }

    //TODO: Only for debugging
    int *my_socket = new->data_p;
    printf("Socket %i added\n", *my_socket);

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
//TODO: Do we have to do that much locking?
    int
linked_list_remove ( int index, linked_list_t *linked_list_p )
{
    list_node_t *cur, *prev, *next;

    for(cur=prev=linked_list_p->first; cur != NULL; prev=cur, cur=cur->next_p){
        next = cur->next_p;

        // lock prev, cur and next
        pthread_mutex_lock(&(prev->mutex));

        // we can't lock cur if cur == prev
        if(cur != prev){
            pthread_mutex_lock(&(cur->mutex));
        }

        // we can't lock the next node if there's only one
        if(next != NULL){
            pthread_mutex_lock(&(next->mutex));
        }

        if(cur->index == index){
            //reset first node if we delete start node
            if(cur->index == linked_list_p->first->index){
                linked_list_p->first = linked_list_p->first->next_p; 
            }
            //reset last node
            else if(cur->index == linked_list_p->last->index){
                linked_list_p->last = prev;
            }

            prev->next_p=cur -> next_p; 

            // unlock prev, cur and next
            pthread_mutex_unlock(&(prev->mutex));

            // we can't unlock cur if cur == prev
            if(cur != prev){
                pthread_mutex_unlock(&(cur->mutex));
            }

            // we can't unlock the next node if there's only one
            if(next != NULL){
                pthread_mutex_unlock(&(next->mutex));
            }

            free(cur);
            break;
        }
        // break is index is out of range
        else if(cur->index > index){
            // unlock prev, cur and next
            pthread_mutex_unlock(&(prev->mutex));

            // we can't unlock cur if cur == prev
            if(cur != prev){
                pthread_mutex_unlock(&(cur->mutex));
            }

            // we can't unlock the next node if there's only one
            if(next != NULL){
                pthread_mutex_unlock(&(next->mutex));
            }
            break; 
        }

        // unlock prev, cur
        pthread_mutex_unlock(&(prev->mutex));

        // we can't unlock cur if cur == prev
        if(cur != prev){
            pthread_mutex_unlock(&(cur->mutex));
        }

        // we can't unlock the next node if there's only one
        if(next != NULL){
            pthread_mutex_unlock(&(next->mutex));
        }
    }
}		/* -----  end of function linked_list_remove  ----- */
