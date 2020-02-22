/***********************************************************************
name: Oleg Kryachun
	readable -- recursively count readable files.
description:
	See CS 360 Files and Directories lecture for details.
***********************************************************************/

/* Includes and definitions */
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

void traverseDir(char *inputPath, struct stat *s, struct dirent*, DIR *, int *count);
int checkReadable(struct stat *s);
/**********************************************************************
Take as input some path. Recursively count the files that are readable.
If a file is not readable, do not count it. Return the negative value
of an error codes you may get. However, consider what errors you want
to respond to.
**********************************************************************/

// main readable function that calls the recursive traverseDir function
int readable(char* inputPath) {
	struct stat buf, *s = &buf;
	struct dirent *dir;
	DIR *dirptr;
	int count = 0;
	if(inputPath == NULL)
		traverseDir(".", s, dir, dirptr, &count);  //set path to cwd
	else
		traverseDir(inputPath, s, dir, dirptr, &count);
	if(errno == EACCES && count == 0) return 0;	//if first file is not readable
	if(errno == ENOTDIR) return -1*EACCES;	//if not dir, return errno EACCESS (13)
	if(errno != 0 && count == 0) return (-1*errno);	//if errno is set readable files is zero
	return count;
}

void traverseDir(char *inputPath, struct stat *s, struct dirent *dir, DIR *dirptr, int *count) {
	char *path;
	if(stat(inputPath, s) == 0) { //if inputPath can be opened
		if(S_ISDIR(s->st_mode) && checkReadable(s)) {
			if(S_ISLNK(s->st_mode)) return; //if symbolic link then return
		    dirptr = opendir(inputPath);
			if(errno != 0) {
				fprintf(stderr, "Value of errno 1: %d\n", errno);
				perror("Error printed by perror\n");
			}
			//while there are files to read
			while((dir = readdir(dirptr)) != NULL) {
				if(strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name,"..") != 0) {  //ignore . and ..
					size_t size = (strlen(inputPath)+strlen(dir->d_name)+2);	//set size for new path
					path = calloc(sizeof(char),size);
					path = strcpy(path,inputPath);
					path[strlen(inputPath)] = '/';
					path = strcat(path,dir->d_name);	//append paths
					traverseDir(path, s, dir, dirptr, count);
					free(path);
				}
			}
			free(dirptr);
			return;
		} else if(S_ISREG(s->st_mode) && checkReadable(s)) {
			if(S_ISLNK(s->st_mode)) return; 	//if symbolic link then return
			(*count)++;
			return;
		}
	} else {	//file doesn't exist or can't be accessed
		fprintf(stderr, "Value of errno 2: %d\n", errno);
		perror("Error printed by perror\n");
		return;
	}
}

//check if the file has reading privledges
int checkReadable(struct stat *s) {
	if((s->st_mode & S_IRUSR && s->st_uid == geteuid()) ||
		(s->st_mode & S_IRGRP && s->st_gid == getegid()) ||
		(s->st_mode & S_IROTH))
		return 1;
	return 0;
}
