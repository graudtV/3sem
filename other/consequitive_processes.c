/* Creates N consequitive processes
 * N is passed as first option in console */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <assert.h>

#define FREQUENT_FEATURES_SINGLE_FILE
#include "../frequent_features.h"

void create_conseq_proc(int nchildren)
{
	int n;
	for (n = nchildren; n > 0; --n) {
		gid_t res = fork();
		if (res == 0) {
			printf("proc: pid = %6d, ppid = %6d\n", getpid(), getppid());
			continue;
		}
		else if (res == -1)
			error("error: unable to create child");
		wait(NULL);
		break;
	}
	if (n < nchildren)
		exit(EXIT_SUCCESS);
}

/* Recursive version
 * The non-recursive one is better */
void create_conseq_proc_rec(int n)
{
	if (n > 0) {
		gid_t res = fork();
		if (res == -1)
			error("error: unable to create child");
		if (res == 0) {
			printf("child: pid = %6d, ppid = %6d\n", getpid(), getppid());
			create_conseq_proc_rec(n - 1);
			exit(EXIT_SUCCESS);
		}
		assert(wait(NULL) != -1);
	}
}


int main(int argc, char *argv[])
{
	int nchildren = 0;

	if (argc != 2)
		error("incorrect usage: expected 1 parameter");
	if (sscanf(argv[1], "%d", &nchildren) != 1)
		error("incorrect usage: expected number");

	printf("main proc: pid = %6d, ppid = %6d\n\n", getpid(), getppid());
	create_conseq_proc(nchildren);

}