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

#include "list.h"
#include <stdlib.h>


void createList( List l){
	l[0] = NULL;
}

int isEmptyList ( List l ){
	return (l[0] == NULL);
}

pos list_first ( List l ){
	return 0;
}

pos list_next ( pos p, List l ){
	if((p >= (MAX_LIST - 1)) || (p < 0)){	// Limits of an array
		return 0;						// Not valid position
	}
	else{
		return p + 1;
	}
}

pos list_last ( List l ){
	int i;
	for(i=0; l[i]!=NULL;){
		i++;
	}
	return i;
}

int list_getElement (elem * e, pos p, List l){
	if(l[p] == NULL){
		return 0;		// Invalid position, last position
	}
	else{
		*e = *l[p];
		return 1;		// The element could be got
	}
}


int list_insert (elem e, List l){
	elem *p=(elem *)malloc (sizeof(elem));
	pos lst = list_last(l);
	pos new_lst = list_next(lst, l);

	if (p==NULL){
		free(p);
		return 0;
	}
	if( lst == (MAX_LIST - 1) ){
		free(p);
		return 0;
	}
	else{
		*p=e; 	// copies the contents to the global var (as memcpy)
		l[lst] = p;
		l[new_lst] = NULL;
		return 1;
	}
}

int list_remove( pos p, List l ){

	if(p>=list_first(l) && p<=list_last(l)){
		free(l[p]);
		while(l[p]!=NULL){
			l[p] = l[list_next(p, l)];
			p = list_next(p, l);
		}
		return 1;
	}
	else{
		return 0;
	}

}


pos list_find_size ( size_t size, List l ){
	/*
		Find an element by its size
	*/

	pos position = list_first(l);
	elem e;
	int tmp = list_getElement(&e, position, l);


	while(tmp && (e.size != size)){
		position = list_next(position, l);
		tmp = list_getElement(&e, position, l);
	}

	if(tmp){
		return position;	// If the position is not the last
	}
	else{
		return -1;			// If the element was not found
	}

}


pos list_find_key ( int key, List l ){
	/*
		Find an element by its key
	*/

	pos position = list_first(l);
	elem e;
	int tmp = list_getElement(&e, position, l);


	while(tmp && (e.key != key)){
		position = list_next(position, l);
		tmp = list_getElement(&e, position, l);
	}

	if(tmp){
		return position;	// If the position is not the last
	}
	else{
		return -1;			// If the element was not found
	}

}

pos list_find_address ( void * address, List l ){

	/*
		Find an element by its address
	*/
	pos position = list_first(l);
	elem e;
	int tmp = list_getElement(&e, position, l);


	while(tmp && (e.address != address)){
		position = list_next(position, l);
		tmp = list_getElement(&e, position, l);
	}

	if(tmp){
		return position;	// If the position is not the last
	}
	else{
		return -1;			// If the element was not found
	}

}

pos list_find_name (char * file_name, List l){

    pos position = list_first(l);
    elem e;
    int tmp = list_getElement(&e, position, l);

    while (tmp && (strcmp(e.file_name, file_name))){
        position = list_next(position, l);
        tmp = list_getElement(&e, position, l);
    }
    if (tmp){
        return position;
    }
    else{
        return -1;
    }
}

elem list_createElement (alloc_t method, void * address, size_t size, char * time, char * file_name,
						 int file_descriptor, int key ){
	/*
		Create an element and copy in the correspondent fields the info
	*/
	elem e;

	e.method = method;
	e.address = address;
	e.size = size;
	strcpy(e.time, time);
	strcpy(e.file_name, file_name);
	e.file_descriptor = file_descriptor;
	e.key = key;

	return e;
}



void showList_method( alloc_t method, List l ){

	/*
		Show the part of the list depending on the allocation method
	*/

	pos p = list_first(l);
	elem e;
	int got = 1;
	while(got){
		got = list_getElement(&e, p, l);
		if (got && e.method == method){
			switch(method){
				case MALLOC: printf("%p: size:%d . malloc %s\n", e.address, (int) e.size , e.time ); break;
				case MMAP: printf("%p: size:%d . mmap %s (fd: %d) %s\n", e.address, (int) e.size, e.file_name, e.file_descriptor, e.time ); break;
				case SHARED: printf("%p: size:%d . shared memory (key: %d) %s\n", e.address, (int) e.size, e.key, e.time ); break;
			}
		}
		p = list_next(p, l);
	}
}


int list_remove_size (size_t size, List l){

    pos position = list_find_size(size, l);

    if (position == -1){
        return 0;
    }
    else{
        return list_remove(position, l);
    }
}

int list_remove_key (int key, List l){

    pos position = list_find_key(key, l);

    if (position == -1){
        return 0;
   	}
    else{
        return list_remove(position, l);
    }
}

int list_remove_address (void * address, List l){

    pos position = list_find_address(address, l);

    if (position == -1){
        return 0;
    }
    else{
    	return list_remove(position, l);
    }
}

int list_remove_name (char * file_name, List l){

    pos position = list_find_name(file_name, l);

    if (position == -1){
        return 0;
    }
    else{
    	if(l[position]->method == MMAP)
        	return list_remove(position, l);
    	else
    		return list_remove(position, l);

    }
}


// *******************FUNCTIONS THAT RETURN ADDRESSES**********************

void * list_find_address_fromkey(key_t k, List l){

	pos p;

	p = list_find_key((int) k, l);

	if(p!=(-1)){
		return (void *) l[p]->address;
	}
	else{
		return NULL;
	}
}

void * list_find_address_fromname(size_t * size, char * name, List l){

	pos p;

	p = list_find_name(name, l);

	if(p!=(-1)){
		*size = l[p]->size;
		return (void *) l[p]->address;
	}
	else{
		return NULL;
	}
}

void * list_find_address_fromsize(size_t size, List l){

	pos p;

	p = list_find_size(size, l);

	if(p!=(-1)){
		return (void *) l[p]->address;
	}
	else{
		return NULL;
	}
}
