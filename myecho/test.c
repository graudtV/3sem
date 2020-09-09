#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *test_strings[] = {
	"abc",
	"abc def",
	"abc  def  klm",
	"abc\tdef",
	"-n abc",
	"-nn abc",
	"-nnnnnn abc",
	"-n -n abc",
	"-nn -nn -nn abc",
	"-nm abc",
	"-nmn abc",
	"-n -nmn abc",
	"- abc",
	"- -n abc",
	"-m abc"
};

#define MAXLINE 1024
#define ARR_SZ(arr) (sizeof (arr) / sizeof (*arr))

#define MYECHO "./myecho"
#define BENCHMARK_ECHO "/bin/echo"

int main()
{
	int nerr = 0;
	char cmd1[MAXLINE] = "";
	char cmd2[MAXLINE] = "";

	for (int i = 0; i < ARR_SZ(test_strings); ++i) {

		strcpy(cmd1, MYECHO " > out1 ");
		strcpy(cmd2, BENCHMARK_ECHO " > out2 ");
		strcat(cmd1, test_strings[i]);
		strcat(cmd2, test_strings[i]);

		//printf("'%s' and '%s'\n", cmd1, cmd2);

		system(cmd1);
		system(cmd2);


		if (system("diff out1 out2 > /dev/null")) {
			fprintf(stderr, "difference in commands '%s'"
				" and '%s':\n", cmd1, cmd2);
			fprintf(stderr,  MYECHO " output is '");
			system("cat out1");
			fprintf(stderr, "', but " BENCHMARK_ECHO " output"
				" is '");
			system("cat out2");
			fprintf(stderr, "'\n");
				
			++nerr;
		}
	}

	if (nerr)
		fprintf(stderr, "test failed: %d errors found\n", nerr);
	return nerr;
}