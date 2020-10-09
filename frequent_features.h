/* Полезные функции */
#ifndef _FREQUENT_FEATURES_H_
#define _FREQUENT_FEATURES_H_

#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>

/*  Prints info message to fout according to fmt
 *  If macro PROGRAM_NAME is defined, prints PROGRAM_NAME
 * before error message (i.e. output looks like "progname: info_message")
 *  If PROGRAM_NAME is defined, it should be a string literal or
 * should be defined as a name of previously declared global variable
 * with type char * / const char * / etc. The second way (using global variable)
 * allows to set programm name in runtime, modifying global variable
 *  Symbol '\n' is added after the message automatically */
void info(FILE* fout, const char *fmt, ...)
{
#ifdef PROGRAM_NAME
	fprintf(fout, "%s: ", (PROGRAM_NAME));
#endif

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
#ifdef PROGRAM_NAME
	fprintf(stderr, PROGRAM_NAME ": ");
#endif

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

#endif // _FREQUENT_FEATURES_H_