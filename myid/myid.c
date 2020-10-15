#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>

#include <stdio.h>
#include <errno.h>
#include <ctype.h>

#define FREQUENT_FEATURES_SINGLE_FILE
#include "../frequent_features.h"

/* Returns passwd entry, terminates programm if user doesnot exist
 * or any other error occures */
struct passwd *getpwuid_s(uid_t uid)
{
	errno = 0;
	struct passwd *passwd_entry = getpwuid(uid);
	if (!passwd_entry)
		error((errno == 0 || errno == ENOENT || errno == ESRCH)
			? "%ld: no such user"
			: "getpwuid() terminated with unknow for uid=%ld",
			(long) uid);
	return passwd_entry;
}

/* Returns passwd entry, terminates programm if user doesnot exist
 * or any other error occures */
struct passwd *getpwnam_s(const char *username)
{
	errno = 0;
	struct passwd *passwd_entry = getpwnam(username);
	if (!passwd_entry)
		error((errno == 0 || errno == ENOENT || errno == ESRCH)
			? "'%s': no such user"
			: "getpwnam() terminated with unknow error for user '%s'",
			username);
	return passwd_entry;
}

/* Returns group entry, terminates programm if group doesnot exist
 * or any other error occures */
struct group *getgrgid_s(gid_t gid)
{
	errno = 0;
	struct group *group_entry = getgrgid(gid);
	if (!group_entry)
		error((errno == 0 || errno == ENOENT || errno == ESRCH)
			? "'%ld': no such group"
			: "getgrgid() terminated with unknow error for gid=%ld",
			(long) gid);
	return group_entry;
}

/* Печатает groups для текущего процесса */
void print_current_process_groups()
{
	gid_t egid = getegid();
	int ngroups = getgroups(0, NULL);
	gid_t groups[ngroups];

	if (getgroups(ngroups, groups) != ngroups)
		error("fatal error: getgroups() failed");

	/* Первым печатаем группу с egid */
	printf("%ld(%s)", (long) egid, getgrgid_s(egid)->gr_name);

	/* Теперь печатаем все остальные, egid исключаем, если попадется */
	for (gid_t *pgid = groups; pgid < groups + ngroups; ++pgid)
		if (*pgid != egid)
			printf(",%ld(%s)", (long) *pgid, getgrgid_s(*pgid)->gr_name);
}

/*  Печатает groups для пользователя по имени username
 *  basegid - будет подставлен в качестве второго аргумента
 * в вызове getgrouplist внутри функции. Теоретически, это должна быть
 * основная группа пользователя (но по факту результат вообще не
 * зависит от передаваемого basegid) */
void print_groups(const char *username, gid_t basegid)
{
	const char * const bad_alloc_msg = "fatal error: not enough memory";

	int ngroups = 1;
	gid_t *groups = malloc(ngroups * sizeof groups[0]);
	if (!groups)
		error(bad_alloc_msg);
	
	while (getgrouplist(username, basegid, groups, &ngroups) == -1) // -1 - not enough size
		if (!(groups = realloc(groups, (ngroups*=2) * sizeof groups[0])))
			error(bad_alloc_msg);

	printf("%ld(%s)", (long) groups[0], getgrgid_s(groups[0])->gr_name);
	for (gid_t *pgid = groups + 1; pgid < groups + ngroups; ++pgid)
		printf(",%ld(%s)", (long) *pgid, getgrgid_s(*pgid)->gr_name);

	free(groups);
}

int main(int argc, char *argv[])
{
	PROGRAM_NAME = argv[0];
	++argv;

	uid_t uid = 0;
	gid_t gid = 0;
	struct passwd *passwd_entry = NULL;
	struct group *group_entry = NULL;

	if (argc == 1)
		uid = getuid();
	else if (isdigit(**argv)) // задан uid
		sscanf(*argv, "%d", &uid);
	else // задан user name
		uid = (passwd_entry = getpwnam_s(*argv))->pw_uid;
	
	if (!passwd_entry)
		passwd_entry = getpwuid_s(uid);
	gid = passwd_entry->pw_gid;
	group_entry = getgrgid_s(gid);

	printf("uid=%ld(%s)", (long) uid, passwd_entry->pw_name);
	printf(" gid=%ld(%s)", (long) gid, group_entry->gr_name);
	
	printf(" groups=");
	if (argc == 1)
		print_current_process_groups();
	else
		print_groups(passwd_entry->pw_name, gid);
	printf("\n");

	return 0;
}