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

#ifndef TRANSLATIONS_H

#define TRANSLATIONS_H

typedef enum {ACTIVE, STOPPED, SIGNALED, TERMINATED, UNKNOWN} state_t;

int getSignal(char * signal);

char * nameStatus(state_t status);

state_t enumStatus(int status);

char * nameSignal(int signal);

#endif
