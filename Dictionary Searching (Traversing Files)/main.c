#include <stdio.h>

//test program that test the searching algorithm with the two dictionaries provided
int ok(char *, int);

int main() {
	int x = ok("webster_16", word, 16);
	int x = ok("tiny_9", word, 9);
	if(x > 0)
		printf("Success search, word was found on line:  %d\n", x);
	else
		printf("Something went wrong, word not found: %d\n", x);
	return 0;
}
