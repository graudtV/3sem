#include <stdio.h>
#include <sys/stat.h>
#include <string.h>

#define FREQUENT_FEATURES_SINGLE_FILE
#include "../frequent_features.h"

typedef enum { F_ERR = -1, F_REG, F_DIR, F_OTHER } ftype_t;

/*  Returns F_ERR if pathname is not a file or if an error occured,
 * one of other constants in ftype_t otherwise */
ftype_t ftype(const char *pathname)
{
	struct stat buf;
	if (stat(pathname, &buf) == -1)
		return F_ERR;
	switch (buf.st_mode & S_IFMT) {
	case S_IFDIR: return F_DIR;
	case S_IFREG: return F_REG;
	default: return F_OTHER;
	}
}

#define skip_file(...) { info(stderr, __VA_ARGS__); continue; }

int main(int argc, char *argv[])
{
	bool multiple_files = (argc >= 4);
	int ifd, ofd;
	struct stat srcstat, dststat;
	const char *srcpath;
	const char *target = argv[argc - 1];

	PROGRAM_NAME = "mycp";
	if (argc == 1)
		error("missing file operand");
	if (argc == 2)
		error("missing destination file operand after '%s'", argv[1]);
	if (multiple_files && ftype(target) != F_DIR)
		error("target '%s' is not a directory", target);

	while (--argc > 1) {
		srcpath = *++argv;
		if ((ifd = open(srcpath, O_RDONLY)) == -1)
			skip_file("cannot open file '%s': %s", srcpath, strerror(errno));
		if (fstat(ifd, &srcstat) == -1)
			skip_file("cannot get stat about file '%s': %s", srcpath, strerror(errno));
		if (S_ISDIR(srcstat.st_mode))
			skip_file("ommiting directory '%s'", srcpath);

		char dstpath[strlen(srcpath) + strlen(target) + 2]; // +1 for '\0', +1 for '/'
		if (multiple_files || ftype(target) == F_DIR) {
			strcpy(dstpath, target);
			strcat(dstpath, "/");
			strcat(dstpath, srcpath);
		} else
			strcpy(dstpath, target);

		if ((ofd = open(dstpath, O_WRONLY | O_CREAT, srcstat.st_mode)) == -1)
			skip_file("cannot open file '%s': %s", dstpath, strerror(errno));
		if (fstat(ofd, &dststat) == -1)
			skip_file("cannot get stat about file '%s': %s", dstpath, strerror(errno));
		if (srcstat.st_ino == dststat.st_ino)
			skip_file("'%s' and '%s' are the same file", srcpath, dstpath);
		if (copyfile(ifd, ofd) == -1)
			skip_file("error while copying file '%s': %s", srcpath, strerror(errno));
		if (close(ifd) == -1)
			error("cannot close file %s: %s", srcpath, strerror(errno));
	}
	return 0;
}