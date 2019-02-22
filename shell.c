/*
TITLE: OPERATING SYSTEMS LABS
AUTHOR 1: MARTIÑO RIVERA DOURADO
AUTHOR 2: CARMEN CORRALES CAMELLO
DATE: 13/12/2017
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <signal.h>
//LISTS
#include "list.h"
#include "searchList.h"
#include "listproc.h"

// Constant values

#define CMD_LENGTH 70
#define NAME_LENGTH PATH_MAX
#define PATH_LENGTH PATH_MAX
#define REC_TAM 2048
#define LEERCOMPLETO ((ssize_t)-1)

// Global type definition

struct COMMAND {

	char * name;
	void (* function)(char **);

};

// Global variable for recursivity

int recursive_flag = 0;				// Initialized as false


// Global variables for the lists

List allocList;						// Allocated blocks list
procList backgList;					// Background process list

// Global variable

int globalVar;







/******************************FUNCTIONS*************************************/

int isDirectory (char * file){

	/*
	 * Objective: This function checks whether the file is a directory or not.
	 * */

	struct stat s;

	if (lstat (file, &s) == -1)
		return 0;

	return S_ISDIR (s.st_mode);


}

char TipoFichero (mode_t m){

	/*
	 * Objective: This function returns a character depending on the type of file.
	 * */

	switch (m&S_IFMT) { /*and bit a bit con los bits de formato,0170000 */
		case S_IFSOCK:  return 's';  /*socket */
		case S_IFLNK:   return 'l';    /*symbolic link*/
		case S_IFREG:   return '-';   /* fichero normal*/
		case S_IFBLK:   return 'b';   /*block device*/
		case S_IFDIR:   return 'd';   /*directorio */
		case S_IFCHR:   return 'c';   /*char  device*/
		case S_IFIFO:   return 'p';   /*pipe*/
		default: return '?';   /*desconocido, no deberia aparecer*/
	}
}


char * getPerms (mode_t m){

	/*
	 * Objective: This function returns an string meaning the permissions of a file.
	 * */

	static char permisos[12];
	strcpy (permisos,"---------- ");

	permisos[0]=TipoFichero(m);
	if (m&S_IRUSR) permisos[1]='r';  /*propietario*/
	if (m&S_IWUSR) permisos[2]='w';
	if (m&S_IXUSR) permisos[3]='x';
	if (m&S_IRGRP) permisos[4]='r';   /*grupo*/
	if (m&S_IWGRP) permisos[5]='w';
	if (m&S_IXGRP) permisos[6]='x';
	if (m&S_IROTH) permisos[7]='r';   /*resto*/
	if (m&S_IWOTH) permisos[8]='w';
	if (m&S_IXOTH) permisos[9]='x';
	if (m&S_ISUID) permisos[3]='s';  /*setuid, setgid y stickybit*/
	if (m&S_ISGID) permisos[6]='s';
	if (m&S_ISVTX) permisos[9]='t';
	return (permisos);
}

char * getUser (uid_t u){

	/*
	 * Objective: This function RETURNS an array with the name of the user of the file .
	 * */

	struct passwd * p;
	p=getpwuid(u);
	if (p==NULL)
		return ("Unknown");
	else
		return (p-> pw_name);
}

char * getGroup (gid_t g){

	/*
	 * Objective: This function RETURNS an array with the name of the group of the file .
	 * */

	struct group * p;
	p=getgrgid(g);
	if (p==NULL)
		return ("Unknown");
	else
		return (p-> gr_name);
}

char * getDate (struct timespec st){

	/*
	 * Objective: This function RETURNS an array with the date of the last modification of the file .
	 * */

	static char stringDate[16];
	strftime(stringDate, 16, "%b %d %R", (localtime(&st.tv_sec) ));
	return stringDate;
}

void do_info(char *file){

	/*
	 * Objective: This function displays the info of a file .
	 * */


	struct stat s;

	int inoNumber;
	char * perm;
	int linkNumber;
	char * user;
	char * group;
	int size;
	char * date;

	if (lstat(file, &s)==-1){
		printf("Error: cannot access %s: %s\n",file, strerror(errno));
		return;
	}

	inoNumber = (int) s.st_ino;
	linkNumber = (int) s.st_nlink;
	perm=getPerms(s.st_mode);
	user=getUser(s.st_uid);
	group=getGroup(s.st_gid);
	size = (int) s.st_size;
	date=getDate(s.st_atim);


	printf ("%d %s %d %s %s %d %s %s", inoNumber, perm, linkNumber, user, group, size, date, file);

	if (S_ISLNK(s.st_mode)){

		char linkName[NAME_LENGTH];
		realpath(file, linkName);
		if (linkName != NULL){
			printf(" -> %s\n", linkName );
		}
		else{
			printf("\nError: cannot follow link %s: %s\n",file, strerror(errno));
		}

  	}
	else printf ("\n");

}

void info (char * args[]){

	/*
	 * Objective: This function processes the command info .
	 * */

	int i;
	for(i=0; args[i]!=NULL; i++)
		do_info(args[i]);
}


void recursive(char * args[]){

	/*
	 * Objective: This function processes the command recursive, to change the recursive flag
	 * 				or display its state.
	 * */

	if(args[0]==NULL){
		if(recursive_flag)
			printf("ON\n");
		else
			printf("OFF\n");
	}
	else if(!strcmp(args[0],"ON")){
		recursive_flag = 1;
	}
	else if(!strcmp(args[0],"OFF")){
		recursive_flag = 0;
	}
	else{
		printf("Invalid arguments\n");
	}

}


void do_list (char * file, int longListing, char * currentDir){

	/*
	 * Objective: This function lists the info of a file. If LONGLISTING is true, the file will be listed
	 * 				like info does.
	 * 				If RECURSIVE_FLAG is true, a directory will be listed with all its contents recusively.
	 * */


	if(!strcmp(file, currentDir)){
		strcpy(file,".");
	}

	if(recursive_flag && isDirectory(file)){

		DIR * d = opendir(file);							// File descriptor of directory
		struct dirent * dirEntry;
		char absolutePath[PATH_LENGTH];
		char * fileName;

		if(d == NULL){
			printf("Error: cannot access %s: %s\n",file, strerror(errno));
		}
		else{
			while((dirEntry=readdir(d))!= NULL){
				fileName = dirEntry->d_name;
				if(strcmp(fileName, ".") && (strcmp(fileName, ".."))){
					strcpy(absolutePath, file);
					strcat(absolutePath,"/");
					strcat(absolutePath, fileName);
					do_list(absolutePath, longListing, currentDir);
				}
			}
		}

		closedir(d);
		return;
	}
	if(isDirectory(file)){

		DIR * d = opendir(file);							// File descriptor of directory
		struct dirent * dirEntry;
		char absolutePath[PATH_LENGTH];
		char * fileName;

		if(d == NULL){
			printf("Error: cannot access %s: %s\n",file, strerror(errno));
			return;
		}
		while((dirEntry=readdir(d))!= NULL){
			fileName = dirEntry->d_name;
			if(strcmp(fileName, ".") && (strcmp(fileName, ".."))){
				strcpy(absolutePath, file);
				strcat(absolutePath,"/");
				strcat(absolutePath, fileName);

				if(longListing){
					do_info(absolutePath);
				}
				else{
					struct stat s;

					if (lstat(absolutePath, &s)==-1){
						printf("Error: cannot access %s: %s\n",absolutePath, strerror(errno));
					}
					else{
						int size = (int) s.st_size;

						printf("%d %s\n", size, fileName);
					}

				}
			}
		}
		closedir(d);
		return;
	}
	// If not directory
	if(longListing){
		do_info(file);
		return;
	}
	struct stat s;

	if (lstat(file, &s)==-1){
		printf("Error: cannot access %s: %s\n",file, strerror(errno));
	}
	else{
		int size = (int) s.st_size;

		printf("%d %s\n", size, file);
	}

}


void list (char * args[]){

	/*
	 * Objective: This function processes the command list .
	 * */

	int longListing = 0;
	static char currentDir[NAME_LENGTH];
	if(getcwd(currentDir,NAME_LENGTH)==NULL){
		printf("\nError: cannot access current directory: %s\n", strerror(errno));
	}

	// Check first position
	if(args[0]==NULL){
		do_list(currentDir, longListing, currentDir);
	}
	else if(!strcmp(args[0], "-l")){
			  longListing = 1;			// If parameter -l is given
			  args = args + 1;
			  if(args[0]==NULL){
					do_list(currentDir, longListing, currentDir);
			  }
	}

	// Check next positions
	int i;
	for(i=0; args[i]!=NULL; i++){
		do_list(args[i], longListing, currentDir);
	}
}



void do_eliminate(char * file, int force){

	/*
	 * Objective: This function eliminates a file or an empty directory. If FORCE is true,
	 * 				full directories
	 * */

	int isDir = isDirectory(file);

	if(force && isDir){
		DIR * d = opendir(file);
		struct dirent * dirEntry;
		char absolutePath[PATH_LENGTH];
		char * fileName;

		while((dirEntry=readdir(d))!=NULL){
			fileName = dirEntry->d_name;
			if(strcmp(fileName, ".") && (strcmp(fileName, ".."))){
				strcpy(absolutePath, file);
				strcat(absolutePath,"/");
				strcat(absolutePath, fileName);
				do_eliminate(absolutePath, force);
			}
		}

		closedir(d);

		if((rmdir(file)==-1)){
			printf("\nError: cannot eliminate the file %s: %s\n", file, strerror(errno));
		}


	}
	else{
		if(isDir){
			if((rmdir(file)==-1)){
				printf("\nError: cannot eliminate the file %s: %s\n", file, strerror(errno));
			}
		}
		else{
			if ((unlink(file))==-1)	{
				printf("\nError: cannot eliminate the file %s: %s\n", file, strerror(errno));
			}
		}

	}

}


void eliminate(char * args[]){

	/*
	 * Objective: This function processes the eliminate command .
	 * */

	int force = 0;

	if(args[0]!=NULL){
		if(!strcmp(args[0], "-f")){
			force = 1;
			args = args + 1;
		}
		if(args[0]!=NULL){
			do_eliminate(args[0], force);
		}
	}

}


int TrocearCadena(char * cadena, char * trozos[]){

	// Provided function. It splits the input string (cadena) into args (trozos)

	int i=1;
	if ((trozos[0]=strtok(cadena," \n\t"))==NULL)
		return 0;
	while ((trozos[i]=strtok(NULL," \n\t"))!=NULL)
		i++;
	return i;
}



void logins(){
	/*
	 * Objective: This function displays the logins.
	 * */

	printf("\n\tcarmen.corralesc, martino.rivera.dourado\n");
}

void names(){
	/*
	 * Objective: This function displays the names.
	 * */

	printf("\n\tCarmen Corrales Camello, Martiño Rivera Dourado\n");
}

void pid(char * args[]){
	/*
	 * Objective: This function gets the flags of the command and displays the process identifier.
	 * */


	if(args[0]==NULL){
		printf("PID of process executing the shell: %d\n", getpid());
	}
	else if(strcmp(args[0], "-p")==0){
		if (getppid() == 0) printf("PID of the parent process in a different PID namespace:\n");
		printf("PID of the parent process: %d\n", getppid());
	}
	else{
		printf("Invalid arguments\n");

	}
}

void autores(char * args[]){

	/*
	 * Objective: This function works with the flags of a command and displays the authors information.
	 * */

	if(args[0]==NULL){
		names();
		logins();
	}
	else if(strcmp(args[0],"-l")==0){
		logins();
	}
	else if(strcmp(args[0], "-n")==0){
		names();
	}
	else{
		printf("Invalid arguments\n");
	}
	printf("\n");

}

void end (char * args[]){
	/*
		Objective: To end the shell
	*/

	exit(0);

}

void gettime(char * string){
	/*
		Objective: To get this instant time
	*/
	time_t now;
	now = time(0);
	strftime(string, 30, "%a %b %d %T %Y", localtime(&now));
}


int isNumber(char * str){

	int answer = 0;
	int i = 0;

	if(str!=NULL){
		answer = 1;
		while(str[i]!='\n' && str[i]!='\0'){
			answer = answer && (isdigit(str[i]) || (str[i]=='-'));	// We check that all characters are digits
			i++;
		}
	}
	return answer;
}



void mallocDealloc (char *str){

	/*
		Objective: Do the deallocate part of the command malloc
	*/

	void * addr;
	int tam;

	if (str==NULL){
		showList_method(MALLOC, allocList);
		return;
	}
	if (!isNumber(str)){
		printf("Invalid arguments\n");
		return;
	}

	sscanf(str, "%d", &tam);				// Cast to int from string

	if ((addr=list_find_address_fromsize(tam, allocList))!=NULL){
		printf("deallocated %d at %p\n", tam, addr);
		free(addr);
		list_remove_size(tam, allocList);
	}
	else{
		showList_method(MALLOC, allocList);
	}
}


void myMalloc (char * args[]){

	/*
		Objective: Execute the malloc command, allocate memory in the process space
	*/


	if (args[0]==NULL){
		showList_method(MALLOC, allocList);
	}
	else if(isNumber(args[0])){
		int inserted;
		int tam;
		char now[30];
		sscanf(args[0], "%d", &tam);				// Cast to int from string
		void * address = malloc(tam);
		gettime(now);
		if(address==NULL){
			printf("Error: cannot allocate %d: %s\n",tam, strerror(errno));
		}
		else{
			printf("allocated %d at %p\n", tam, address);
			elem newElem = list_createElement(MALLOC, address, tam, now, "", 0, 0);
			inserted = list_insert(newElem, allocList);					// Insertion in the list of allocation
			if(!inserted){
				printf("Error: cannot insert element in the list\n");
				return;
			}
		}
	}
	else if(!strcmp(args[0], "-deallocate")){		// Call to deallocate
			mallocDealloc(args[1]);
    }
	else{
		printf("Invalid arguments\n");
	}
}



void * mmapFile (char * file, int protection){

	/*
		Objective: To map a file in the shell's space
	*/

	int df, map=MAP_PRIVATE, mode=O_RDONLY, inserted;
	struct stat s;
	void *p;
	char now[30];

	if (protection & PROT_WRITE)
		mode=O_RDWR;
	if (stat(file,&s)==-1 || (df=open(file, mode))==-1)
		return NULL;
	if ((p=mmap (NULL,s.st_size, protection, map, df, 0))==MAP_FAILED)
		return NULL;

	// Insertion in the list
	gettime(now);
	elem newElem = list_createElement(MMAP, p, s.st_size, now, file, df, 0);
	inserted = list_insert(newElem, allocList);
	if(!inserted){
		printf("Error: cannot insert element in the list\n");
		return NULL;
	}

	return p;
}


void mmapDealloc (char *str){


	/*
		Objective: Do the deallocate part of mmap
	*/

	void * addr;
	size_t size;

	if (str==NULL){
		showList_method(MMAP, allocList);
		return;
	}

	if ((addr=list_find_address_fromname(&size, str, allocList))!=NULL){
		printf ("file %s unmapped at %p\n", str, addr);
		munmap(addr, size);
		list_remove_name(str, allocList);				// Removal from the list from the name
	}
	else{
		showList_method(MMAP, allocList);
	}
}

void myMmap (char * args[]){

	/*
		Objective: Process the mmap command with permissions rights
	*/

	char *perm;
	void *p;
	int protection=0;

	if (args[0]==NULL){
		showList_method(MMAP, allocList);
	}
	else if(!strcmp(args[0], "-deallocate")){
    	mmapDealloc(args[1]);
   	}
	else{
		// Process the permissions
		if ((perm=args[1])!=NULL && strlen(perm)<4) {
			if (strchr(perm,'r')!=NULL)
				protection |= PROT_READ;
			if (strchr(perm,'w')!=NULL)
				protection |= PROT_WRITE;
			if (strchr(perm,'x')!=NULL)
				protection |= PROT_EXEC;
		}
		if ((p=mmapFile(args[0],protection))==NULL)
			perror ("Imposible to map file");
		else
			printf ("file %s mapped at %p\n", args[0], p);
	}
}


void * ObtainMemoryShmget (key_t key, off_t tam){

	/*
		Objective: To obtain the shared memory address
	*/

	void * p;
	int aux,id,flags=0777;
	struct shmid_ds s;
	char now[30];
	int inserted;

	if (tam) /*si tam no es 0 la crea en modo exclusivo */
		flags=flags | IPC_CREAT | IPC_EXCL;

	/*si tam es 0 intenta acceder a una ya creada*/
	if (key==IPC_PRIVATE){	/*no nos vale*/
		errno=EINVAL;
		return NULL;
	}

	if ((id=shmget(key, tam, flags))==-1)
		return (NULL);

	if ((p=shmat(id,NULL,0))==(void*) -1){
		aux=errno;	/*si se ha creado y no se puede mapear*/
		if (tam)	/*se borra */
			shmctl(id,IPC_RMID,NULL);
		errno=aux;
		return (NULL);
	}

	shmctl (id,IPC_STAT,&s);

	// Insertion in the list
	gettime(now);
	elem newElem = list_createElement(SHARED, p, s.shm_segsz, now, "", 0, key);
	inserted = list_insert(newElem, allocList);
	if(!inserted){
		printf("Error: cannot insert element in the list\n");
		return NULL;
	}

	return (p);
}



void sharednew (char * args[]){

	/*
		Objective: Process the command of sharednew memory
	*/

	key_t k;
	off_t tam = 0;
	void * p;


	if (args[0]==NULL || args[1]==NULL){
		showList_method(SHARED, allocList);
		return;
	}

	k = (key_t) atoi(args[0]);					// We get the key (cl)

	if (args[1]!=NULL)							// We get the size
		tam=(off_t) atoll(args[1]);

	if ((p=ObtainMemoryShmget(k,tam))==NULL)	// If the address is not valid
		perror ("Cannot allocate");

	else
		printf ("Allocated shared memory (key %d) at %p\n",k,p);
}


void sharedDealloc (char *str){

	/*
		Objective: Do the deallocate part of shared
	*/

	key_t k;
	void * addr;

	if (str==NULL){
		showList_method(SHARED, allocList);
		return;
	}

	k = (key_t) atoi(str);

	if ((addr=list_find_address_fromkey(k,allocList))!=NULL){
		shmdt (addr);
		list_remove_key(k, allocList);
	}
	else{
		printf ("No shared memory of key %d allocated \n", (int) k);
	}
}

void shared (char * args[]){

	/*
		Objective: Process the shared command
	*/

	key_t k;
	void * p;

	if(args[0] == NULL){
		showList_method(SHARED, allocList);
		return;
	}
	if(!strcmp(args[0], "-deallocate")){
		sharedDealloc (args[1]);
		return;
	}

	k = (key_t) atoi(args[0]);				// We get the key (cl)

	if ((p=ObtainMemoryShmget(k,0))==NULL)
		perror ("Cannot allocate");

	else
		printf ("Allocated shared memory (key %d) at %p\n",k,p);
}


void rmkey (char * args[]){

	/*
		Objective: Remove the shared memory key of the system
	*/

	key_t k;
	int id;
	char * key = args[0];

	if (key==NULL || (k=(key_t) strtoul(key,NULL,10))==IPC_PRIVATE){
		printf ("rmkey: invalid key\n");
		return;
	}

	if ((id=shmget(k,0,0666))==-1){
		perror ("shmget: impossible to obtain shared memory");
		return;
	}

	if (shmctl(id,IPC_RMID,NULL)==-1)
		perror ("shmctl: impossible to eliminate shared memory\n");
}

void allocation (char * args[]){

	/*
		Objective: Show the allocated space
	*/

	showList_method(MALLOC, allocList);
	showList_method(MMAP, allocList);
	showList_method(SHARED, allocList);

}

void deallocate (char * args[]){

	/*
		Objective: Deallocates a specified address from the allocated space
 	*/

	void * addr;
	pos p;
	elem e;

	if(args[0]==NULL){
		allocation(args);
		return;
	}

	sscanf(args[0], "%p", &addr);

	if((p = list_find_address(addr, allocList))==-1){
		allocation(args);
	}
	else{
		list_getElement(&e, p, allocList);
		switch(e.method){
			case MALLOC:
					free(e.address);
					list_remove_size(e.size, allocList);
					break;
			case MMAP:
					munmap(e.address, e.size);
					list_remove_name(e.file_name, allocList);
					break;
			case SHARED:
					shmdt (e.address);
					list_remove_key(e.key, allocList);
					break;
		}
	}
}

void mem (char * args[]){

	/*
		Objective: Prints the memory addresses required in the wording
	*/

	// Local variables
	int var1;
	char * var2;
	char var3;

	printf("Program functions:\n");
		printf("\t autores: %p\n", autores);
		printf("\t deallocate: %p\n", deallocate);
		printf("\t pid: %p\n", pid);

	printf("Global variables:\n");
		printf("\t allocList: %p\n", &allocList);
		printf("\t recursive_flag: %p\n", &recursive_flag);
		printf("\t globalVar: %p\n", &globalVar);

	printf("Local variables:\n");
		printf("\t var1: %p\n", &var1);
		printf("\t var2: %p\n", &var2);
		printf("\t var3: %p\n", &var3);
}


void myMemdump (char * args[]){

	/*
		Objective: Shows the contents in ASCII and hexadecimal of a memory address in the shell's
					space.
	*/

	int cont, limit, start, i;
	char * address;

	if(args[0]==NULL){
		printf("Invalid arguments\n");
		return;
	}
	else{
		sscanf(args[0], "%p", &address);
	}

	if(args[1]==NULL){
		cont = 25;
	}
	else{
		sscanf(args[1], "%d", &cont);
	}


	start = 0;

	while(cont>0){

		if(cont >= 25){
			limit = start + 25;
		}
		else{
			limit = start + (cont % 25);
		}

		for(i = start; i<limit; i++){
			if (isprint(*(address+i))){
				printf(" %2c ", *(address+i));
			}
			else
				printf("    ");
		}

		printf("\n");

		for(i = start; i<limit; i++){
			printf(" %2X ", *(address+i));
		}

		printf("\n");

		cont = cont - (limit - start);
		start = limit+1;

	}

}


void myRecursivefunction (int n){

	/*
		Objective: Function that calls recursively to itself
	*/

	char automatic [REC_TAM];
	static char static_char [REC_TAM];


	printf("parameter n:%d in %p\n", n, &n);
	printf("static array in: %p \n", static_char);
	printf("atomatic array in: %p\n\n", automatic);

	n--;
	if (n>0)
		myRecursivefunction(n);
}

void recursivefunction(char * args[]){

	/*
		Objective: Process the command recursivefunction
	*/

	if(args[0]==NULL){
		return;
	}
	if(isNumber(args[0])){
		int n = (int) atoi(args[0]);
		myRecursivefunction(n);
	}
	else{
		printf("Invalid arguments\n");
	}
}


ssize_t readFile (char * fich, void *p, ssize_t n){

	/*
		Objective: Reads a file and returns its size
	*/

	ssize_t nleidos,tam=n;
	int df, aux;
	struct stat s;

	if (stat (fich,&s)==-1 || (df=open(fich,O_RDONLY))==-1)
		return ((ssize_t)-1);
	if (n==LEERCOMPLETO)
		tam=(ssize_t) s.st_size;
	if ((nleidos=read(df,p, tam))==-1){
		aux=errno;
		close(df);
		errno=aux;
		return ((ssize_t)-1);
	}
	close (df);
	return (nleidos);
}



void myRead(char * args[]){

	/*
		Objective: Process the command read of the shell
	*/

	int cont = 0;
	char * addr;
	char * file = args[0];

	if(args[0]==NULL){
		return;
	}
	if(args[1]==NULL){
		return;
	}

	sscanf(args[1], "%p", &addr);				// Get the address

	if(args[2]==NULL){							// Cont value
		if(readFile(file, addr, -1)==-1){		// Read all
			perror("Error when rading file");
		}
	}
	else{
		sscanf(args[2], "%d", &cont);
		if(readFile(file, addr, cont)!=cont){		// Read all
			perror("Error when reading file");
		}
	}

}


void myWrite(char * args[]){

	/*
		Objective: Process the command write of the shell
	*/

	int fd;
	int cont = 0;
	char * addr;
	int overwrite = 0;
	int flags;

	if(args[0]==NULL){
		return;
	}
	if(args[1]==NULL){
		return;
	}
	if(args[3]!=NULL){
		overwrite = (!strcmp(args[3], "-o"));
	}
	sscanf(args[1], "%p", &addr);
	sscanf(args[2], "%d", &cont);


	if(overwrite){
		flags = O_CREAT | O_WRONLY;
	}
	else{
		flags = O_CREAT | O_WRONLY | O_EXCL;
	}

	if((fd = open(args[0], flags))==-1){
		perror("Error when writing to file or creating file");
		return;
	}


	if(write(fd, addr, cont)==-1){
		perror("Error when writing to file");
	}


	close(fd);

}


void mySetpriority(char * args[]){

	/*
		Objective: Sets the priority of a process
	*/

	int priority;

	if(args[0]==NULL){
		return;
	}
	else if(!isNumber(args[0])){
		printf("Invalid arguments\n");
		return;
	}

	int processid;

	sscanf(args[0], "%d", &processid);

	if(args[1]==NULL){
		if((priority = getpriority(PRIO_PROCESS, (id_t)processid))==-1){
			perror("Error when showing priority"); return;
		}
		printf("Priority of %d: %d\n", processid, priority);
		return;
	}

	if(!isNumber(args[1])){
		printf("Invalid arguments\n");
		return;
	}

	sscanf(args[1], "%d", &priority);

	if(setpriority(PRIO_PROCESS, (id_t)processid, priority)==-1){
		perror("Error when setting priority"); return;
	}
}

void myFork(char * args[]){

	/*
		Objective: Process the command fork
	*/

	pid_t pid;
	int status;

	pid = fork();

	switch((int)pid){

		case (-1): perror("Error creating child");	// Parent's code
				   break;
		case (0): break;				// Child's code
		default: wait(&status);
				break;

	}
}


void mySearchList(char * args[]){

	/*
		Objective: Processes the command searchlist (adds or displays the current list where
					executable files are searched)
	*/

	if(args[0]==NULL){
		SearchListShow();	// Displays the list
		return;
	}
	if(!strcmp(args[0], "-path")){
		SearchListAddPath();		// Adds to the list the contents in $PATH
		return;
	}
	if(args[0][0]=='+'){
		if(isDirectory(args[0]+1)){
			SearchListAddDir(args[0]+1);	// To add the directory to the list
		}
		else
			printf("Invalid argument: Not a directory\n");

		return;
	}

	char * pathname;
	// If it's an executable file
	if((pathname = SearchExecutable(args[0]))==NULL){
		printf("Not such executable file\n");
	}
	else{
		printf("%s\n", pathname);
	}

}


void execExProgram(char * args[]){
	/*
	 * Objective: This function just executes an external program with its arguments
	 				replacing the current process with that one
	 *
	 * */

	if(args[0]==NULL)
		return;

	int i;
	char * prog = SearchExecutable(args[0]);

	if(prog==NULL){
		printf("Cannot execute: %s not found\n", args[0]); return;
	}

	for(i=0; args[i]!=NULL;i++);							// We get the last position of the arguments

	if(args[i-1][0]=='@'){
		int priority;
		sscanf(args[i-1]+1, "%d", &priority);
		if(setpriority(PRIO_PROCESS, getpid(), priority)==-1){
			perror("Error when setting priority"); return;
		}
		args[i-1]=NULL;
	}

	if(execv(prog, args)==-1){								// If the external program cannot execute
		perror("Error when executing external program"); return;
	}
}



void process (char * args[], int background){

	/*
		Objective: Executes an executable file in background or foreground (specified in the
					parameter background)
	*/

	pid_t pid;
	int prio;
	state_t state;
	char time[30];

	if ((pid=fork())==0){
		execExProgram(args);
		exit(255);
	}
    if (!background)
    	waitpid(pid,NULL,0);
    else{
    	if((prio = getpriority(PRIO_PROCESS, (id_t)pid))==-1){
			perror("Error when getting priority"); return;
		}
    	gettime(time);
  		state = ACTIVE;
		elemP * e = proc_createElement(pid, prio, args, time, state, 0);
		proc_insert(e, backgList);
    }
}

void background(char * args[]){

	/*
		Objective: Processes the background command (executes in background a process)
	*/

	if(args[0]==NULL)
		return;

	process(args, 1);

}


void jobs (char * args[]){
	/*
		Objective: Show the list of current process in background
	*/

	proc_showList(backgList);

}

void proc (char * args[]){

	/*
		Objective: Shows the information of a given process (by pid)
	*/

	pid_t pid;
	posP position;

	if(args[0]==NULL){
		jobs(args);return;
	}

	sscanf(args[0], "%d", &pid);

	if((position=proc_find_pid(backgList, pid))==NULL){	// If the process is not in the list, show the list of processes
		jobs(args); return;
	}

	proc_updateList(backgList);
	elemP e = proc_getElement(position);
	proc_show(e);

}

void clearjobs(char * args[]){

	/*
		Objective: Clean the list of processes
	*/
	proc_clear(backgList);
}


int processCommand(char * cmd, struct COMMAND C []){

	/*
	 * Objective: This function just processes the command and calls the apropiate functions.
	 *
	 * It RETURNS 1 if the command means to exit the process.
	 *
	 * */


	int nargs;
	char* args[CMD_LENGTH];				// We assume the user could introduce separated characters (that is, CMD_LENGTH args)
	int i;

	nargs = TrocearCadena(cmd, args);		// We store the number of args in the command
	if (nargs==0)
		return 0;


	for(i = 0; C[i].name!=NULL ; i++){		// It goes from 0 to the last element of the array

		if ((!strcmp(args[0], C[i].name)) ){
			C[i].function(args+1);
			break;
		}
	}

	if (C[i].name==NULL){					// 	If the command is not found
		process(args, 0);
	}

	return 0;

}


void readCommand(char* command){
	/*
	 * Objective: This function just displays a prompt and returns a string read from stdin
	 * */

	printf("-> ");
	fgets(command, CMD_LENGTH, stdin);
}




/****************************************MAIN*PROGRAM*********************************************/


int main(){

	char cmd[CMD_LENGTH];						// Command processed
	int finished=0;								// Logic value that represents the end of main loop

	//COMMANDS OF THE SHELL
	struct COMMAND C [] = {

		{"autores", autores},
		{"pid", pid},
		{"info", info},
		{"recursive", recursive},
		{"list", list},
		{"eliminate", eliminate},
		{"malloc", myMalloc},
		{"mmap", myMmap},
		{"sharednew", sharednew},
		{"shared", shared},
		{"rmkey", rmkey},
		{"allocation", allocation},
		{"deallocate", deallocate},
		{"mem", mem},
		{"memdump", myMemdump},
		{"recursivefunction", recursivefunction},
		{"read", myRead},
		{"write", myWrite},
		{"setpriority", mySetpriority},
		{"fork", myFork},
		{"searchlist", mySearchList},
		{"proc", proc},
		{"clearjobs", clearjobs},
		{"background", background},
		{"jobs", jobs},
		{"exec", execExProgram},				// Spetial commands
		{"fin", end},
		{"exit", end},
		{"end", end},
		{NULL, NULL}
	};											// We initialize the array of possible commands


	createList(allocList);						// We intialize the lists
	backgList = proc_createList();



	// MAIN LOOP

	while(!finished){
		readCommand(cmd);
		finished = processCommand(cmd, C);			// If a end process is called
	}

	return 0;
}
