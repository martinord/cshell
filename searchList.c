/*
TITLE: OPERATING SYSTEMS LABS
AUTHOR 1: MARTIÑO RIVERA DOURADO
AUTHOR 2: CARMEN CORRALES CAMELLO
DATE: 13/12/2017
*/


/*
 * 	Searchlist implementation
 * */

#include "searchList.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAXNOMBRE 1024
#define MAXSEARCHLIST 128


char *searchlist[MAXSEARCHLIST]={NULL};

int SearchListAddDir(char * dir){

	int i=0;
	char * p;

	while (i<MAXSEARCHLIST-2 && searchlist[i]!=NULL)
		i++;

	if (i==MAXSEARCHLIST-2){
		errno=ENOSPC;
		return -1;
	} /*does not fit*/

	if ((p=strdup(dir))==NULL)
		return -1;

	searchlist[i]=p;
	searchlist[i+1]=NULL;

	return 0;
}


void SearchListNew(){

	int i;

	for (i=0; searchlist[i]!=NULL; i++){
		free(searchlist[i]);
		searchlist[i]=NULL;
	}
}


void SearchListShow(){

	int i;

	for (i=0; searchlist[i]!=NULL; i++)
		printf ("\t- %s\n",searchlist[i]);

}


void SearchListAddPath(){

	char *aux;
	char *p;

	if ((p=getenv("PATH"))==NULL){
		printf ("Impossible to obtain PATH of the system\n");
		return;
	}

	aux=strdup(p);

	if ((p=strtok (aux,":"))!=NULL && SearchListAddDir(p)==-1)
		printf ("Impossible to add %s: %s\n", p, strerror(errno));

	while ((p=strtok(NULL,":"))!=NULL)
		if (SearchListAddDir(p)==-1){
			printf ("Impossible to add %s: %s\n", p, strerror(errno));
			break;
		}

	free(aux);

}


char * SearchExecutable(char * ejec){

	static char aux[MAXNOMBRE];
	int i;
	struct stat s;

	if (ejec==NULL)
		return NULL;

	if (ejec[0]=='/' || !strncmp (ejec,"./",2) || !strncmp (ejec,"../",2)){
		if (stat(ejec,&s)!=-1)
			return ejec;
		else
			return NULL;
	}

	for (i=0;searchlist[i]!=NULL; i++){
		sprintf(aux,"%s/%s",searchlist[i],ejec);
		if (stat(aux,&s)!=-1)
			return aux;
	}

	return NULL;
}
