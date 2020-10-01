/* Creates N parallel processes
 * N is passed as first option in console */

#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "../frequent_features.h"

int main(int argc, char *argv[])
{
	int nchildren = 0;

	if (argc != 2)
		error("incorrect usage: expected 1 parameter");
	if (sscanf(argv[1], "%d", &nchildren) != 1)
		error("incorrect usage: expected number");

	printf("main proc: pid = %6d, ppid = %6d\n\n", getpid(), getppid());

	pid_t ret = 0;

	for (int i = 0; i < nchildren; ++i) {
		if ((ret = fork()) == 0) { // child
			printf("child #%2d: pid = %6d, ppid = %6d\n", i + 1, getpid(), getppid());
			return 0; // child ends
		}
		if (ret == - 1) // error
			error("unable to create child #%2d: %s", i + 1, strerror(errno));
	}
	for (int i = 0; i < nchildren; ++i)
		wait(NULL);

	return 0;
}