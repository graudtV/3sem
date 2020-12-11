#include <stdio.h>

#define FREQUENT_FEATURES_SINGLE_FILE
#include "../frequent_features.h"


int main(int argc, char *argv[])
{
	PROGRAM_NAME = "mycat";

	if (argc == 1 && copyfile(STDOUT_FILENO, STDOUT_FILENO) == -1)
		error("error while copying stdin to stdout: %s", strerror(errno));
	else
		while (--argc > 0) {
			int ifd;
			if ((ifd = open(*++argv, O_RDONLY)) == -1)
				error("cannot open file %s: %s", *argv, strerror(errno));
			if (copyfile(ifd, STDOUT_FILENO) == -1)
				error("error while copying file %s: %s", *argv, strerror(errno));
			if (close(ifd) == -1)
				error("cannot close file %s: %s", *argv, strerror(errno));
		}

	return 0;
}