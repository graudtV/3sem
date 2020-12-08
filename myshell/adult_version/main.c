#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <signal.h>
#include "../../console_colors.h"

#define FREQUENT_FEATURES_SINGLE_FILE
#include "../../frequent_features.h"

const int MAX_TOKEN = 256; // max argument length
const int MAX_ARGS = 1000; // max arguments in one command

#define BASH_NAME "ebash"
const char *greeting_msg = CONSOLE_GREEN("***** Welcome to ") CONSOLE_PURPLE(BASH_NAME) CONSOLE_GREEN(" !!! *****");
const char *leaving_msg = CONSOLE_GREEN("***** Thanks for using ") CONSOLE_PURPLE(BASH_NAME) CONSOLE_GREEN(" !!! *****");
const char *error_prompt = CONSOLE_PURPLE("-" BASH_NAME);
const char *prompt = CONSOLE_PURPLE(BASH_NAME "$ ");
const char *short_prompt = CONSOLE_PURPLE("> ");

typedef enum {
	TOK_UNKNOWN, TOK_TOO_LONG_ARG, TOK_EOF, TOK_ARG, TOK_PIPE, TOK_NEWLINE
} token_type_t;

struct {
	token_type_t type;
	char name[MAX_TOKEN + 1]; // +1 - for '\0'		
} current_token;

token_type_t _get_token(char *buf, size_t maxbuf)
{
	size_t buf_sz = 0;
	token_type_t type = TOK_UNKNOWN;
	
	int c = 0;
	while (isspace(c = getchar()) && c != '\n') // skip spaces
		;
	while (buf_sz < maxbuf - 1 && c != EOF && c != '|' && !isspace(c) ) {
		buf[buf_sz++] = c;
		c = getchar();
	}
	if (buf_sz == 0) {
		if (c == '|')
			return TOK_PIPE;
		if (c == '\n')
			return TOK_NEWLINE;
		else if (c == EOF)
			return TOK_EOF;
		else
			return TOK_UNKNOWN;
	}
	ungetc(c, stdin);
	buf[buf_sz++] = '\0';
	return (buf_sz == maxbuf && !isspace(c)) ? TOK_TOO_LONG_ARG : TOK_ARG;
}

token_type_t get_token()
{
	return (current_token.type = _get_token(current_token.name, MAX_TOKEN + 1)); // +1 - for '\0';
}

void skip_empty_inputs()
{
	do {
		printf("%s", prompt);
	} while (get_token() == TOK_NEWLINE);	
}

void skip_empty_inputs_short()
{
	do {
		printf("%s", short_prompt);
	} while (get_token() == TOK_NEWLINE);	
}

/* returns 0 if success */
int parse_command_args(char **args, size_t *nargs, size_t maxargs)
{
	size_t args_sz = 0;
	while (1) {
		if (args_sz == 0 && current_token.type == TOK_NEWLINE)
			skip_empty_inputs_short();
		if (current_token.type != TOK_ARG)
			break;
		if (args_sz >= maxargs) {
			info(stderr, "too long command");
			info(stderr, "info: max number of arguments in one command = %d", maxargs);
			return 1;
		}
		args[args_sz++] = strdup(current_token.name);
		get_token();
	}

	if (current_token.type == TOK_TOO_LONG_ARG) {
		info(stderr, "too long argument: %s...", current_token.name);
		info(stderr, "info: max argument lenght = %d", MAX_TOKEN);
		return 1;
	}
	if (args_sz == 0 && current_token.type == TOK_PIPE) {
		info(stderr, "unexpected '|' symbol");
		return 1;
	}
	assert(!(args_sz == 0 && current_token.type != TOK_EOF));

	*nargs = args_sz;
	return 0; // success
}

token_type_t process_next_command()
{
	int input_fd = STDIN_FILENO;
	int output_fd = -1;

	char *args[MAX_ARGS + 1]; // +1 - for ending NULL
	size_t args_sz = 0;

	skip_empty_inputs();

	while (1) {
		if (parse_command_args(args, &args_sz, MAX_ARGS) != 0) {
			if (current_token.type == TOK_EOF)
				return TOK_EOF;
			rewind(stdin);
			skip_empty_inputs();
			continue;
		}
		args[args_sz] = NULL;
		if (current_token.type != TOK_PIPE) {
			execute(args[0], args, input_fd, NULL);
			break;
		} else { // TOK_PIPE => transfering output
			execute(args[0], args, input_fd, &output_fd);
			if (input_fd != STDIN_FILENO) // child now has his own access to this file, can release in parent
				close(input_fd);
			input_fd = output_fd;
			get_token(); // skip TOK_PIPE
		}
		
	}
	while (wait(NULL) != -1)
		;
		
	return current_token.type;
}

void exit_shell(int unused)
{
	printf("\n\n%s\n", leaving_msg);
	exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
	PROGRAM_NAME = error_prompt;
	signal(SIGINT, exit_shell);

	printf("%s\n\n", greeting_msg);
	while (process_next_command() != TOK_EOF)
		;
	exit_shell(0);
	return 0;
}