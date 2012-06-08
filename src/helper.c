/*
 * =====================================================================================
 *
 *       Filename:  helper.c
 *
 *    Description:  Helper functions.
 *
 *        Version:  1.0
 *        Created:  06/08/2012 11:16:02 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Andreas Brönnimann
 *   Organization:  v-net.ch
 *
 * =====================================================================================
 */
#include <stdlib.h>

#include <string.h>

#include "chat.h"

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
}               /* -----  end of function substr  ----- */
