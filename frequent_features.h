/* Полезные функции */
#ifndef _FREQUENT_FEATURES_H_
#define _FREQUENT_FEATURES_H_

#include <stdlib.h>
#include <stdarg.h>

/*  Prints error message according to fmt and terminates programm
 *  If macro PROGRAM_NAME is defined, prints PROGRAM_NAME 
 * before error message (PROGRAM_NAME should be a string literal)
 *  Symbol '\n' is added after the message automatically */
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


#endif // _FREQUENT_FEATURES_H_