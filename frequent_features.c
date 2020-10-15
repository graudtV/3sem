#include "frequent_features.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <errno.h>
#include <string.h> // strerror()

const char *PROGRAM_NAME;

/*  Prints info message to fout according to fmt
 *  Global variable PROGRAM_NAME of type const char * can be set to
 * print program name before info message. If PROGRAM_NAME is NULL
 * or empty string (""), program name is not printed. If PROGRAM_NAME
 * is not a valid pointer, behaviour is undefined
 *  Symbol '\n' is added after the message automatically */
void info(FILE* fout, const char *fmt, ...)
{
	if (PROGRAM_NAME && *PROGRAM_NAME)
		fprintf(fout, "%s: ", (PROGRAM_NAME));

	va_list args;
	va_start(args, fmt);
	vfprintf(fout, fmt, args);
	va_end(args);

	fputc('\n', fout);	
}

/*  Same as info, but prints message to stderr
 * and terminates programm after printing */
void error(const char *fmt, ...)
{
	if (PROGRAM_NAME && *PROGRAM_NAME)
		fprintf(stderr, "%s: ", (PROGRAM_NAME));

	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);

	fputc('\n', stderr);
	exit(EXIT_FAILURE);
 }

/*  Copies file with file descriptor fd_src to file with
 * descriptor fd_dst
 *  Returns 0 if success, -1 otherwise */
int copyfile(int fd_src, int fd_dst)
{
	char buf[BUFSIZ];
	int n = 0;

	while ((n = read(fd_src, buf, BUFSIZ)) > 0) {
		char *bufend = buf + n;
		for (char *p = buf; p < bufend; p += n)
			if ((n = write(fd_dst, p, bufend - p)) < 0)
				break;
	}
	return n;
}

/*  Copies file with file descriptor fd_src to file with
 * descriptor fd_dst.
 *  Returns 0 if success, -1 otherwise
 *  On success, writes number of copied bytes, words and lines
 * to struct file_info_t pointed by info */
int copyfile_informative(int fd_src, int fd_dst, struct file_info_t *info)
{
	if (!info)
		return copyfile(fd_src, fd_dst);

	struct file_info_t temp_info = {};

	char buf[BUFSIZ];
	int n = 0;
	bool in_word = false;

	while ((n = read(fd_src, buf, BUFSIZ)) > 0) {
		temp_info.bytes += n;
		char *bufend = buf + n;	
		for (char *p = buf; p < bufend; p += n)
			if ((n = write(fd_dst, p, bufend - p)) < 0)
				break;

		for (char *p = buf; p < bufend; ++p) {
			if (*p == '\n')
				++temp_info.lines;
			if (!isgraph(*p)) {
				in_word = false;
			} else if(!in_word) {
				in_word = true;
				++temp_info.words;				
			}
		}
	}
	if (n == 0) { // success
		info->bytes = temp_info.bytes;
		info->words = temp_info.words;
		info->lines = temp_info.lines;
	}
	return n;
}

/*  Запускает программу с именем file в виде дочернего процесса с возможностью
 * задания собственных stdin и stdout
 *  В качестве стандартного ввода дочернего процесса используется открытый
 * на чтение в текущем процессе файл с дескриптором input_fd. Вывод
 * дочернего процесса отправляется в pipe. Read-конец pipe-а будет доступен
 * в вызывающем процессе по дескриптору, который запишется в *output_fd при
 * успешном завершении execute()
 *  file, argv должны удовлетворять требованиям execvp()
 * (В частности, argv обязан(!) заканчиваться нулевым указателем)
 *
 *  input_fd, в частности, может быть равен 0 (STDIN_FILENO), тогда дочерний
 * процесс будет использовать стандартный ввод, заданный по умолчанию,
 * то есть тот же, что и у текущего процесса
 *  Если output_fd == NULL, дочерний процесс будет использовать стандартный
 * вывод, заданный по умолчанию, то есть тот же, что и у текущего процесса
 *
 *  Note. Вызывающий процесс может закрыть input_fd сразу после вызова
 * execute(), если не собирается пользоваться им сам (закрытие, скорее всего,
 * желательно, иначе и текущий, и дочерний процесс смогут обращаться к одному
 * и тому же файлу, но позиция в файле будет "одна на двоих". Исключение -
 * использование общего stdin)
 *  Note. Не забыть вызвать по wait() на каждый execute()
 */
void execute(const char *file, char * const argv[],
	int input_fd, int *output_fd)
{
	int pipe_read_fd = -1;
	int pipe_write_fd = -1;

	if (output_fd != NULL) {
		int pipe_fds[2];
		if (pipe(pipe_fds) != 0)
			error("cannot create pipe: %s", strerror(errno));
		pipe_read_fd = pipe_fds[0];
		pipe_write_fd = pipe_fds[1];
	}

	pid_t res = fork();
	if (res == 0) {
		if (output_fd != NULL) {
			close(pipe_read_fd);
			if (dup2(pipe_write_fd, STDOUT_FILENO) == -1)
				error("dup2() failed: %s", strerror(errno));
			close(pipe_write_fd);
		}
		if (input_fd != STDIN_FILENO) {
			if ((dup2(input_fd, STDIN_FILENO)) == -1)
				error("dup2() failed: %s", strerror(errno));
			close(input_fd);
		}
		if (execvp(file, argv) == -1)
			error("cannot execute '%s': %s", *argv, strerror(errno));
		/* execvp will not return on success */
		assert(0);
	}
	if (res == -1)
		error("cannot create child process: %s", strerror(errno));
	if (output_fd) {
		close(pipe_write_fd);
		*output_fd = pipe_read_fd;
	}
}

/*  Returns struct with current time, terminates program if error occurs
 * clock_id must meet requirment of clock_gettime() */
struct timespec get_time_s(clockid_t clock_id)
{
	struct timespec time;
	if (clock_gettime(clock_id, &time) == -1)
		error("cannot get time: %s", strerror(errno));
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
	fprintf(fout, "%4lds %4ldms %4ldus\n",
		sec, nsec / (int) 1e6, (nsec / 1000) % 1000);
}