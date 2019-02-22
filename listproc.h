/*
TITLE: OPERATING SYSTEMS LABS
AUTHOR 1: MARTIÑO RIVERA DOURADO
AUTHOR 2: CARMEN CORRALES CAMELLO
DATE: 13/12/2017
*/

/*
 * 	Process list, implementation of linked list with a header node (1)
 *
 * */

#ifndef LISTPROC_H

#define LISTPROC_H

#define MAXLINE 1024		// Maximum number of characters a command line can be called with

#include "translations.h"


typedef struct{
	pid_t pid;				// PID of the process
	int prio;				// Priority
	char cmd [MAXLINE];		// Command
	char date[30];			// Time stamp of the moment of starting the process
	state_t status;			// State of the process, enumeration(translations.h)
	int signal;				// Codified exit-status/stopped/terminated signal of the process

} elemP;

struct node{
	elemP * element;
	struct node * next;
};

typedef struct node * posP;

typedef struct node * procList;


procList proc_createList();

void proc_insert(elemP * e, procList l);

void proc_clear(procList l);

posP proc_find_pid(procList l, pid_t reqPid);

void proc_showList(procList l);

void proc_show(elemP e);

elemP proc_getElement(posP p);

elemP * proc_createElement(pid_t pid, int prio, char * args[], char * date, int status, int signal);

void proc_updateList(procList l);

#endif
