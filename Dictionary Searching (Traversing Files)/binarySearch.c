/***********************************************************************
name: Oleg Kryachun
	ok -- see if a word is in the online dictionary
description: This is a binary search dictionary searching algorithm using only the read and
			 write.
	See CS 360 IO lecture for details.
***********************************************************************/

/* Includes and definitions */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

int binarySearch(int fd, int min, int max, int dictWidth, char *word, char *buffer);
void truncateBuff(char *buffer, int length);

/**********************************************************************
Search for given word in dictionary using file descriptor fd.
Return the line number the word was found on, negative of the last line searched
if not found, or the error number if an error occurs.
**********************************************************************/

int ok(char *dictionaryName, char *word, int dictWidth) {
	//initialize and declare variables that are used throughout this function
	int fd, retVal, errnum, j = 0;
    long i = 0, numLines = 0;
	char buffer[dictWidth];
	char *wordCopy = malloc(sizeof(char)*strlen(word));

	//copy word to wordCopy byte by byte
	while(word[j] != '\0') {
		if(j == dictWidth-1) {	//if length reaches dictWidth, null terminate and break
			wordCopy[j] = '\0';
			break;
		}
		wordCopy[j] = word[j];
		j++;
	}
	//attempt to open dictionary path, if error then print and return errno
	fd = open(dictionaryName, O_RDONLY, 0);
	if(fd < 0) {
		errnum = errno;
		fprintf(stderr, "Value of errno: %d\n", errnum);
		perror("Error printed by perror");
		return (errnum);
	}
	numLines = (lseek(fd, i, SEEK_END))/dictWidth;  //get the number of lines in dictionary
	retVal = binarySearch(fd, 0, numLines, dictWidth, wordCopy, buffer);  //search for word
	close(fd);
	free(wordCopy);
	if(retVal < (-1*numLines))	//if value is nagative (wasnt found) and less than negative value
		++retVal;			   //of lineNum, then add 1 to make lineNum and retVal equal
    return retVal;
}

//recursive function which performs a binary search of the dictionary
int binarySearch(int fd, int min, int max, int dictWidth, char *word, char *buffer) {
    int mid = 0;
    if(max >= min) { //if there is more to search
        if((mid > 0) && ((max - min)/2 < 1))  //if at the beggining of the dictionary
            mid = min + 1;
        else
            mid = (max - min) / 2 + min;  //else normal mid calculation

		lseek(fd, mid * dictWidth, SEEK_SET);	//seek mid position in file
		read(fd, buffer, dictWidth);	//read dictWidth bytes and store in buffer
		truncateBuff(buffer, dictWidth);	//truncate buffer to get rid of excess bytes
		if(strcmp(buffer, word) == 0) //base case
			return ++mid;
		if(strcmp(buffer, word) > 0)  //search left side of dictionary
			return binarySearch(fd, min, mid-1, dictWidth, word, buffer);
		if(strcmp(buffer, word) < 0)  //search right side of dictionary
			return binarySearch(fd, mid+1, max, dictWidth, word, buffer);
	}
	if(max == 0 || min == 0) //if searched to the beggining of the dictionary and doesnt exist
		return -1;
	return (min*-1);  //else return negative value of min
}

//starting from dictWidth, remove extra spaces and new lines for buffer
void truncateBuff(char *buffer, int length) {
    length--;
    while(buffer[length] == ' ' || buffer[length] == '\n')
        length--;
    buffer[++length] = '\0';
}

