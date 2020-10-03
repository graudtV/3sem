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
	printf("Usage: %s [-tTq] program program_options ...\n", PROGRAM_NAME);
	printf("\t-t\t--\tprint cpu time\n");
	printf("\t-T\t--\tprint real time\n");
	printf("\t-q\t--\tquiet mode\n");
	exit(EXIT_FAILURE);
}

struct timespec get_time_s(clockid_t clock_id)
{
	struct timespec time;
	if (clock_gettime(clock_id, &time) == -1)
		error("cannot get current time: %s", strerror(errno));
	return time;
}

void print_time_diff(struct timespec end, struct timespec begin, FILE *fout)
{
	time_t sec = end.tv_sec - begin.tv_sec;
	long nsec = end.tv_nsec - begin.tv_nsec;
	while (nsec < 0) {
		++sec;
		nsec += (int) 1e9;
	}
	fprintf(stderr, "%4lds %4ldus %4ldms\n",
		sec, nsec / (int) 1e6, (nsec / 1000) % 1000);
}

struct file_info_t {
	size_t bytes;
	long words;
	long lines;
};

int copy_file2(int fd_src, int fd_dst, struct file_info_t *info)
{
	if (!info)
		return copy_file(fd_src, fd_dst);

	info->bytes = info->words = info->lines = 0;

	char buf[BUFSIZ];
	int n = 0;

	while ((n = read(fd_src, buf, BUFSIZ)) > 0) {
		if ((n = write(fd_dst, buf, n)) < 0)
			break;
		info->bytes += n;
	}
	return n;
}

int main(int argc, char *argv[])
{
	int opt_cpu_time = 0;
	int opt_real_time = 0;
	int opt_quiet = 0;
	int opt = 0;

	while ((opt = getopt(argc, argv, "tTq")) != -1) {
		switch (opt) {
		case 't': opt_cpu_time = 1; break;
		case 'T': opt_real_time = 1; break;
		case 'q': opt_quiet = 1; break;
		default: usage_error();
		}
	}

	argc -= optind;
	argv += optind;

	if (!argc) {
		info(stderr, "no program to execute specified");
		usage_error();
	}

	int pipe_fds[2];
	if (pipe(pipe_fds) != 0)
		error("cannot create pipe: %s", strerror(errno));
	int pipe_read_fd = pipe_fds[0];
	int pipe_write_fd = pipe_fds[1];

	struct timespec real_time_start = get_time_s(CLOCK_MONOTONIC);
	struct timespec cpu_time_start = get_time_s(CLOCK_PROCESS_CPUTIME_ID);
	
	gid_t ret = fork();
	if (ret == 0) {
		close(pipe_read_fd);
		if (dup2(pipe_write_fd, STDOUT_FILENO) == -1) // Теперь stdout тоже будет писать в pipe
			error("cannot create pipe: %s", strerror(errno));
		close(pipe_write_fd); // Теперь в pipe пишет только stdout
		if (execvp(*argv, argv) == -1)
			error("cannot execute %s: %s", *argv, strerror(errno));
		exit(EXIT_SUCCESS);
	}
	close(pipe_write_fd);
	struct file_info_t file_info;
	copy_file2(pipe_read_fd, STDOUT_FILENO, &file_info);
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
	printf("bytes: %zu\n", file_info.bytes);

	return 0;
}