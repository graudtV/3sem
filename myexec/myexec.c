/* myexec: executes programm and prints execution time if specified
 * Usage: ./myexec [-tT] program program_options ...
 * Options:
 * 		-t -- print cpu time
 *		-T -- print real time
 **/

#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define PROGRAM_NAME "myexec"
#include "../frequent_features.h"

void usage_error()
{
	printf("Usage: %s [-tT] program program_options ...\n", PROGRAM_NAME);
	printf("\t-t\t--\tprint cpu time\n");
	printf("\t-T\t--\tprint real time\n");
	exit(EXIT_FAILURE);
}

/* Prints time difference between 'begin' and current moment
 * 'begin' should be get with the same clock_id in clock_gettime() call */
void print_time_diff(clockid_t clock_id, struct timespec *begin, FILE *fout)
{
	struct timespec end;

	if (clock_gettime(clock_id, &end) == -1)
		error("cannot get current time: %s", strerror(errno));
	time_t sec = end.tv_sec - begin->tv_sec;
	long nsec = end.tv_nsec - begin->tv_nsec;
	while (nsec < 0) {
		++sec;
		nsec += (int) 1e9;
	}
	fprintf(stderr, "%4lds %4ldus %4ldms\n",
		sec, nsec / (int) 1e6, (nsec / 1000) % 1000);
}

int main(int argc, char *argv[])
{
	int opt_cpu_time = 0;
	int opt_real_time = 0;
	int opt = 0;

	while ((opt = getopt(argc, argv, "+tT")) != -1) {
		switch (opt) {
		case 't': opt_cpu_time = 1; break;
		case 'T': opt_real_time = 1; break;
		default: usage_error();
		}
	}

	argc -= optind;
	argv += optind;

	if (!argc) {
		info(stderr, "no program to execute specified");
		usage_error();
	}

	const char * const time_error_msg = "cannot get current time";
	struct timespec cpu_time_start;
	struct timespec real_time_start;

	if (opt_real_time)
		if (clock_gettime(CLOCK_MONOTONIC, &real_time_start) != 0)
			error("%s: %s", time_error_msg, strerror(errno));
	if (opt_cpu_time)
		if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &cpu_time_start) != 0)
			error("%s: %s", time_error_msg, strerror(errno));

	gid_t ret = fork();
	if (ret == 0) {
		if (execvp(*argv, argv) == -1) // error occured
			error("cannot execute %s: %s", *argv, strerror(errno));
		exit(EXIT_SUCCESS);
	}
	wait(NULL);

	if (opt_real_time) {
		fprintf(stderr, "real time: ");
		print_time_diff(CLOCK_MONOTONIC, &real_time_start, stderr);
	}
	if (opt_cpu_time) {
		fprintf(stderr, "cpu time:  ");
		print_time_diff(CLOCK_PROCESS_CPUTIME_ID, &cpu_time_start, stderr);
	}

	return 0;
}