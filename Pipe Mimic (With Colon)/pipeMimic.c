/***********************************************************************
name: Oleg Kryachun
	assignment4 -- acts as a pipe using ":" to seperate programs.
description: Imitate pipe using a colon. This implementation involves using fork and exec
	See CS 360 Processes and Exec/Pipes and Signals lecture for helpful tips.
***********************************************************************/

/* Includes and definitions */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>

void runArg(char **);
void forkPipe(char **, char **);
void parseArgs(int argc, char *argv[], char **, char **);
void freeArgs(char **);
/**********************************************************************
./assignment4 <arg1> : <arg2>

    Where: <arg1> and <arg2> are optional parameters that specify the programs
    to be run. If <arg1> is specified but <arg2> is not, then <arg1> should be
    run as though there was not a colon. Same for if <arg2> is specified but
    <arg1> is not.
**********************************************************************/


int main(int argc, char *argv[]){
	char **buff = calloc(sizeof(char*),100); 
	char **buff1 = calloc(sizeof(char*),100);
	if(argc > 1) {
		parseArgs(argc, argv, buff, buff1);
		if(buff[0] != NULL && buff1[0] == NULL)  //if only one argument
			runArg(buff);
		else if(buff1[0] != NULL && buff[0] == NULL) // if only one argument
			runArg(buff1);
		else
			forkPipe(buff, buff1); //if two arguments and a colon
	}
	freeArgs(buff);
	free(buff1);
	return 0;
}

void freeArgs(char **buff) {
	int i=0;
	while(buff[i] != NULL) {
		free(buff[i]);
		i++;
	}
	free(buff);
}

void runArg(char **exec) {
	execvp(*exec, exec);
	if(errno > 0)
		fprintf(stderr, "%s\n", strerror(errno));
}


void forkPipe(char **exec1, char **exec2) {
	int fd[2];
	pid_t pid;
	pipe(fd);	//set pipe
    if(errno > 0)
        fprintf(stderr, "%s\n", strerror(errno));
	pid = fork();
	if(pid == 0) {  //Exec1 writes into pipe
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		close(fd[1]);
		runArg(exec1);	//run first arguments
	} else { //Exec2 reads from pipe
		pid = fork();
		if(pid == 0) {
			close(fd[1]);
			dup2(fd[0], STDIN_FILENO);
			close(fd[0]);
			runArg(exec2);	//run second arguments
		} else {	//child waits for all processes to finish
			close(fd[0]);
			close(fd[1]);
			waitpid(-1, NULL, 0);
			waitpid(-1, NULL, 0);
		}
	}
}

void parseArgs(int argc, char *argv[], char **exec1, char **exec2) {
    int i = 0, j = 1, byte = 0, colonPosition = 0;
	char buff[100] = {0}, buff1[100] = {0};
	while(argv[j][i] != ':' && j < argc) {	//find colon in arguments given
		if(argv[j][i] == '\0') {
			if(j == argc-1) break;
			j++; i=0; continue;
		}
		i++;
	}
	if(argv[j][i] == ':') colonPosition = j; //if colon exists then save position
	if(argv[j][i] == ':' && strlen(argv[j]) > 1) {	//if colon is between a word
		i = 0;
		while(argv[j][i] != ':') {	//split the word
			buff[byte] = argv[j][i];
			i++; byte++;
		}
		buff[byte] = '\0';
		byte = 0; i++;
		while(argv[j][i] != '\0') {	//split the second half of the word
			buff1[byte] = argv[j][i];
			i++; byte++;
		}
		buff1[byte] = '\0';
	}
	j=1;	//reset j to the firs argument
	//copy arguments before colon or if no colon present
	while((j < colonPosition && colonPosition != 0) || (colonPosition == 0 && j < argc)) {
		exec1[j-1] = calloc(sizeof(char), strlen(argv[j])+1);
		strcpy(exec1[j-1], argv[j]);
		j++;
	}
	if(colonPosition == 0) return; //if no colon then don't continue
	if(strlen(buff) > 0) {	//if buff was used
		exec1[j-1] = calloc(sizeof(char), strlen(buff)+1);
		strcpy(exec1[j-1], buff);
	}
	int index = 0;
	if(strlen(buff1) > 0) {	//if buff1 was used
		exec2[index] = calloc(sizeof(char), strlen(buff)+1);
		strcpy(exec2[index], buff1);
		index++;
	}
	j++;
	while(j < argc) {	//save last arguments after colon
		exec2[index] = calloc(sizeof(char), strlen(argv[j])+1);
		strcpy(exec2[index], argv[j]);
		index++; j++;
	}
}
