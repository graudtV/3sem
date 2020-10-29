/* Usefull functions */
/* Functions' descriptions could be found in frequent_features.c */
#ifndef FREQUENT_FEATURES_H_
#define FREQUENT_FEATURES_H_

#include <stdio.h> // for FILE
#include <time.h> // clockid_t

/* May be set to modify info() and error() functions
* behaviour, see info() description */
extern const char *PROGRAM_NAME;

/* Stores information about number of bytes, words and lines in file */
struct file_info_t {
	size_t bytes;
	long words;
	long lines;
};

void info(FILE* fout, const char *fmt, ...);
void error(const char *fmt, ...);
int copyfile(int fd_src, int fd_dst);
int copyfile_informative(int fd_src, int fd_dst, struct file_info_t *info);
void execute(const char *file, char * const argv[], int input_fd, int *output_fd);
struct timespec get_time_s(clockid_t clock_id);
void print_time_diff(struct timespec end, struct timespec begin, FILE *fout);

/* Executes function as a child process
 * Return value of called function is ignored.
 * Note. Do not forget to call wait() for each PARALLEL() */
#define PARALLEL(func_call)	{								\
		pid_t _fork_res;									\
		if ((_fork_res = fork()) == 0) {					\
			func_call;										\
			exit(EXIT_SUCCESS);								\
		} else if (_fork_res == -1)							\
			error("fork() failed: %s", strerror(errno));	\
	}

/*  If macro FREQUENT_FEATURES_SINGLE_FILE is defined,
 * source code of frequent_features.c will be included in place */
#ifdef FREQUENT_FEATURES_SINGLE_FILE
#include "frequent_features.c"
#endif

#endif // FREQUENT_FEATURES_H_