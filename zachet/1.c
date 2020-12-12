#include <stdio.h>
#include <sys/wait.h>

#define FREQUENT_FEATURES_SINGLE_FILE
#include "../frequent_features.h"

int main(int argc, char *argv[])
{
	setvbuf(stdout, NULL, _IONBF, 0);

	while (--argc > 0) {
		++argv;
		for (char *p = *argv; *p; ++p) {
			PARALLEL( putchar(*p) );
			wait(NULL);
		}
		PARALLEL( putchar((argc > 1) ? ' ' : '\n') );
		wait(NULL);
	}

	return 0;
}