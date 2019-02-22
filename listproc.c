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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "listproc.h"


struct node * proc_createNode(){

	struct node * tmp = malloc(sizeof(struct node));

	if(tmp == NULL){
		printf("Out of space\n");
		exit(EXIT_FAILURE);
	}

	return tmp;
}

procList proc_createList(){

	struct node * node = proc_createNode();

	node->next = NULL;

	return node;
}

int proc_isEmptyList(procList l){

	return (l->next == NULL);
}

posP proc_first(procList l){

	posP p;

	if(proc_isEmptyList(l))
		p = l;
	else
		p = l->next;

	return p;
}

posP proc_next(posP p){
	return p->next;
}

int proc_isLast(struct node * p){
	return(p->next == NULL);
}

void proc_insert(elemP * e, procList l){

	struct node * tmp = proc_createNode();
	posP p = proc_first(l);

	while(!proc_isLast(p))
		p = p->next;		// We get the last position

	tmp->element = e;
	tmp->next = p->next;
	p->next = tmp;
}

void proc_clear(procList l){

	if(proc_isEmptyList(l))
		return;

	posP p = proc_first(l);
	posP nextPos;

	while(p != NULL){
		nextPos = p->next;
		free(p);
		p = nextPos;
	}

	l->next = NULL;
}

posP proc_find_pid(procList l, pid_t reqPid){

	posP p = proc_first(l);

	while(p != NULL){
		if(p->element->pid == reqPid)
			break;
		p = p->next;
	}

	return p;
}

elemP * proc_createElement(pid_t pid, int prio, char * args[], char * date,
						   int status, int signal){

	elemP * aux = malloc(sizeof(elemP));
	int i;

	// Copy of values in the element
	aux->pid = pid;
	aux->prio = prio;
	// Copy of cmd field
	strcpy(aux->cmd,"");
	for(i = 0; args[i]!=NULL; i++){
		strcat(aux->cmd, args[i]);
		strcat(aux->cmd, " ");
	}

	strcpy(aux->date, date);
	aux->status = status;
	aux->signal = signal;

	return aux;
}

elemP proc_getElement(posP p){
	return *(p->element);
}

void proc_updateList(procList l){
	/*
		Update the list asking the state and signals to the processes in the list
	*/

	if(proc_isEmptyList(l))
		return;

	posP p = proc_first(l);
	elemP* e;
	int prio, state;

	while(p != NULL){
		e = p->element;

		if((prio = getpriority(PRIO_PROCESS, (id_t)e->pid))!=-1)
			e->prio = prio;

		if (waitpid(e->pid, &state, WNOHANG | WUNTRACED | WCONTINUED)==e->pid){

			e->status = enumStatus(state);

			if(WIFSIGNALED(state))
				e->signal = WTERMSIG(state);
			else if(WIFEXITED(state))
				e->signal = WEXITSTATUS(state);
			else if(WIFSTOPPED(state))
				e->signal = WSTOPSIG(state);
		}
		p = proc_next(p);

	}
}

void proc_show(elemP e){
	/*
		Show the element of the list, the info of a process
	*/

	char signalField[10];
	char * statusField;

	strcpy(signalField, "");

	if(e.status != ACTIVE){												// If the process is NOT active
		if(e.status != TERMINATED){										// If the processes is not terminated
			strcat(signalField, "(");
			strcat(signalField, nameSignal(e.signal));					// We show the signal name
			strcat(signalField, ")");
		}
		else{
			sprintf(signalField, "(%d)", e.signal);						// We get the exit status
		}

	}

	statusField = nameStatus(e.status);									// We get the name of the state from the enumeration value

	printf("%d     %s %s p=%d %s %s\n", e.pid, statusField,
			signalField, e.prio, e.date, e.cmd);
}

void proc_showList(procList l){
	/*
		Show the list of processes
	*/

	if(proc_isEmptyList(l))
		return;
	posP p = proc_first(l);
	elemP e;

	proc_updateList(l);				// Before, we update the list asking all processes their states

	while(p != NULL){
		e = proc_getElement(p);
		proc_show(e);				// Show each independent element
		p = proc_next(p);
	}
}
