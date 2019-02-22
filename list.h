/*
TITLE: OPERATING SYSTEMS LABS
AUTHOR 1: MARTIÑO RIVERA DOURADO
AUTHOR 2: CARMEN CORRALES CAMELLO
DATE: 13/12/2017
*/

/*
 * 	List implementation case 3 (array of pointers), NULL terminated
 *
 * */

#ifndef LIST_H

#define LIST_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <sys/mman.h>

#define MAX_LIST 4096
#define MAX_N PATH_MAX		// Maximum length of name of file


typedef int pos;

typedef enum {MALLOC, MMAP, SHARED} alloc_t;

typedef struct{
	alloc_t method;			// Type of allocation, allocation method
	void * address;			// Memory address
	size_t size;			// Size of allocation
	char time[30];			// Time of allocation
	char file_name[MAX_N];	// Name of the file
	int file_descriptor;	// File descriptor
	int key;				// Key for shared memory

} node;

typedef node elem;

typedef elem * List[MAX_LIST];

void createList( List l);

int isEmptyList ( List l );

pos list_first ( List l );

pos list_next ( pos p, List l );

pos list_last ( List l );

int list_getElement ( elem * e, pos p, List l );

int list_insert (elem e, List l);

int list_remove_size (size_t size, List l);

int list_remove_key (int key, List l);

int list_remove_address (void * address, List l);

elem list_createElement (alloc_t method, void * address, size_t size, char * time, char * file_name,
						 int file_descriptor, int key );

void showList_method ( alloc_t method, List l );

int list_remove_name (char * file_name, List l);

void * list_find_address_fromkey(key_t k, List l);

void * list_find_address_fromname(size_t * size, char * name, List l);

void * list_find_address_fromsize(size_t size, List l);

pos list_find_address ( void * address, List l );

#endif
