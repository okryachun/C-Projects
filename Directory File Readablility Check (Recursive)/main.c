#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>

int readable(char *);

int main() {
	printf("Readable file count: %d\n", readable("/root/wsu"));
	return 0;
}
