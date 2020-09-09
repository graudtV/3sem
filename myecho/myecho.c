#include <stdio.h>
#include <stdbool.h>

/* returns true - если символ новой строки должен выводится
 * returns false - в противном случае
 * pargv должен указывать на argv[0] в main()
 * parse_newline() переводит *pargv на первый неразобранный
 * аргумент */ 
bool parse_newline(char ***pargv)
{
	char **fst_arg = ++*pargv;
	char *p = NULL;

// Для сокращения записи. Получаем синтаксис как в main-е
#define argv (*pargv)

	while (*argv && (*argv)[0] == '-' && (*argv)[1]) { // обработка нескольких аргументов подряд
		for (p = &(*argv)[1]; *p; ++p) // обработка нескольких букв за одним '-'
			if (*p != 'n')
				return !(argv > fst_arg); // В записи 'echo -nm ...' n не должен распознаваться как аргумент 
		++argv;
	}
	return !(argv > fst_arg || p > &(*argv)[1]); // В скобках - условие, что был хотя бы один 'n'

#undef argv
}

int main(int argc, char *argv[])
{
	bool newline = parse_newline(&argv);

	for (; *argv; ++argv)
		printf(*(argv + 1) ? "%s " : "%s", *argv);
	if (newline)
		printf("\n");

	return 0;
}