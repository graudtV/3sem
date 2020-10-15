/* myexec: executes programm and prints execution time if specified
 * Usage: ./myexec [-tT] [-qc] program program_options ...
 * Options:
 * 		-t -- print cpu time
 *		-T -- print real time
 *		-q -- quiet mode (suppress executed program's output)
 *		-c -- count number of lines, words and bytes in output
 **/

#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

#define FREQUENT_FEATURES_SINGLE_FILE
#include "../frequent_features.h"

void usage_error()
{
	fprintf(stderr, "Usage: %s [-tT] [-qc] program program_options ...\n",
		PROGRAM_NAME);
	fprintf(stderr,
		"\t-t\t--\tprint cpu time\n"
		"\t-T\t--\tprint real time\n"
		"\t-q\t--\tquiet mode\n"
		"\t-c\t--\tcount number of lines, words and bytes in output\n");
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	PROGRAM_NAME = argv[0];

	int opt_cpu_time = 0;
	int opt_real_time = 0;
	int opt_quiet = 0;
	int opt_count = 0;
	int opt = 0;

	while ((opt = getopt(argc, argv, "+tTqc")) != -1) {
		switch (opt) {
		case 't': opt_cpu_time = 1; break;
		case 'T': opt_real_time = 1; break;
		case 'q': opt_quiet = 1; break;
		case 'c': opt_count = 1; break;
		default: usage_error();
		}
	}

	argc -= optind;
	argv += optind;

	if (!argc) {
		info(stderr, "no program to execute specified");
		usage_error();
	}

	/* Файл, в который перенаправится вывод дочерней программы после pipe-a
	 * По умолчанию - stdout родителя */
	int output_fd = STDOUT_FILENO;
	if (opt_quiet && ((output_fd = open("/dev/null", O_RDONLY)) == -1))
		error("cannot open /dev/null: %s", strerror(errno));

	struct timespec real_time_start = get_time_s(CLOCK_MONOTONIC);
	struct timespec cpu_time_start = get_time_s(CLOCK_PROCESS_CPUTIME_ID);
	
	int child_pipe_output_fd;
	execute(*argv, argv, STDIN_FILENO, &child_pipe_output_fd);

	struct file_info_t file_info = {};
	copyfile_informative(child_pipe_output_fd, output_fd, &file_info);
	wait(NULL);

	if (opt_real_time) {
		struct timespec real_time_end = get_time_s(CLOCK_MONOTONIC);
		fprintf(stderr, "real time: ");
		print_time_diff(real_time_end, real_time_start, stderr);
	}
	if (opt_cpu_time) {
		struct timespec cpu_time_end = get_time_s(CLOCK_PROCESS_CPUTIME_ID);
		fprintf(stderr, "cpu time:  ");
		print_time_diff(cpu_time_end, cpu_time_start, stderr);
	}	
	if (opt_count)
		fprintf(stderr, "lines: %ld\nwords: %ld\nbytes: %zu\n",
			file_info.lines, file_info.words, file_info.bytes);

	return 0;
}