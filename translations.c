/*
TITLE: OPERATING SYSTEMS LABS
AUTHOR 1: MARTIÑO RIVERA DOURADO
AUTHOR 2: CARMEN CORRALES CAMELLO
DATE: 13/12/2017
*/

/*
 * 	Functions to translate the signals and status from
 *	integers to strings.
 *  Besides, this code supplies the support for status with
 *  enums.
 *
 * */

#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include "translations.h"

/******************************SIGNALS ***************************************/
struct SEN{
	char *nombre;
	int senal;
};


static struct SEN sigstrnum[]={

	{"HUP", SIGHUP},
	{"INT", SIGINT},
	{"QUIT", SIGQUIT},
	{"ILL", SIGILL},
	{"TRAP", SIGTRAP},
	{"ABRT", SIGABRT},
	{"IOT", SIGIOT},
	{"BUS", SIGBUS},
	{"FPE", SIGFPE},
	{"KILL", SIGKILL},
	{"USR1", SIGUSR1},
	{"SEGV", SIGSEGV},
	{"USR2", SIGUSR2},
	{"PIPE", SIGPIPE},
	{"ALRM", SIGALRM},
	{"TERM", SIGTERM},
	{"CHLD", SIGCHLD},
	{"CONT", SIGCONT},
	{"STOP", SIGSTOP},
	{"TSTP", SIGTSTP},
	{"TTIN", SIGTTIN},
	{"TTOU", SIGTTOU},
	{"URG", SIGURG},
	{"XCPU", SIGXCPU},
	{"XFSZ", SIGXFSZ},
	{"VTALRM", SIGVTALRM},
	{"PROF", SIGPROF},
	{"WINCH", SIGWINCH},
	{"IO", SIGIO},
	{"SYS", SIGSYS},

	/*signals that are not everywhere*/

	#ifdef SIGPOLL
		{"POLL", SIGPOLL},
	#endif

	#ifdef SIGPWR
		{"PWR", SIGPWR},
	#endif

	#ifdef SIGEMT
		{"EMT", SIGEMT},
	#endif

	#ifdef SIGINFO
		{"INFO", SIGINFO},
	#endif

	#ifdef SIGSTKFLT
		{"STKFLT", SIGSTKFLT},
	#endif

	#ifdef SIGCLD
		{"CLD", SIGCLD},
	#endif

	#ifdef SIGLOST
		{"LOST", SIGLOST},
	#endif

	#ifdef SIGCANCEL
		{"CANCEL", SIGCANCEL},
	#endif

	#ifdef SIGTHAW
		{"THAW", SIGTHAW},
	#endif

	#ifdef SIGFREEZE
		{"FREEZE", SIGFREEZE},
	#endif

	#ifdef SIGLWP
		{"LWP", SIGLWP},
	#endif

	#ifdef SIGWAITING
		{"WAITING", SIGWAITING},
	#endif

	{NULL,-1}
};	/*final array sigstrnum */


int getSignal(char * signal){

	/*returns the number of signal from the name*/

	int i;

	for (i=0; sigstrnum[i].nombre!=NULL; i++)
		if (!strcmp(signal, sigstrnum[i].nombre))
			return sigstrnum[i].senal;

	return -1;
}

char * nameSignal(int signal){

	/*returns the name of the signal from the number
	 * for places where there's not sig2str*/

	int i;
	for (i=0; sigstrnum[i].nombre!=NULL; i++)
		if (signal==sigstrnum[i].senal)
			return sigstrnum[i].nombre;
	return ("SIGUNKNOWN");
}


char * nameStatus(state_t status){
	/*
		Returns the name of the status from the enumeration value
	*/

	switch(status){
		case TERMINATED: return "TERMINATED"; break;
		case SIGNALED: return "SIGNALED"; break;
		case ACTIVE: return "ACTIVE"; break;
		case STOPPED: return "STOPPED"; break;
		default: return "UNKNOWN";
	}
}


state_t enumStatus(int status){

	/*
		Returns the value of the enumeration of the status from the integer codified status
	*/


	if(WIFEXITED(status))			// If the process has ended
		return TERMINATED;

	if(WIFSIGNALED(status))			// If the process has ended with a signal
		return SIGNALED;

	if(WIFSTOPPED(status))			// If the process has been stopped
		return STOPPED;

    if(WIFCONTINUED(status))
    	return ACTIVE;			// If the process is active

	return UNKNOWN;
}
