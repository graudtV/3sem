#include <stdio.h>
#include <stdbool.h>

int main(int argc, char *argv[])
{
	bool newline = true;

	while (*++argv && (*argv)[0] == '-' && (*argv)[1] == 'n')
		newline = false;

	for (; *argv; ++argv)
		printf(*(argv + 1) ? "%s " : "%s", *argv);
	if (newline)
		printf("\n");

	return 0;
}