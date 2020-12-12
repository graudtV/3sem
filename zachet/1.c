#include <stdio.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define FREQUENT_FEATURES_SINGLE_FILE
#include "../frequent_features.h"

bool letters[256] = {};
int nchildren = 0;
int queue_id;

struct msgbuf_t {
	long mtype;
	bool kill_letter;
};

#define letter_to_type(c) ((c) + 2)
#define letter_is_printed 1

void letter_handler(int c)
{
	struct msgbuf_t msg;
	const struct msgbuf_t msgready = { .mtype = letter_is_printed };

	while (1) {
		if (msgrcv(queue_id, &msg, sizeof msg.kill_letter, letter_to_type(c), 0) == -1)
			error("failed to receive letter: %s", strerror(errno));
		if (msg.kill_letter)
			break;
		putchar(c);
		if (msgsnd(queue_id, &msgready, sizeof msg.kill_letter, 0) == -1)
			error("failed to send answer message: %s", strerror(errno));
	}
}

void send_letter(int c)
{
	if (!letters[c]) {
		PARALLEL( letter_handler(c) );
		letters[c] = true;
		++nchildren;
	}
	struct msgbuf_t msg;
	msg.mtype = letter_to_type(c);
	msg.kill_letter = false;
	if (msgsnd(queue_id, &msg, sizeof msg.kill_letter, 0) == -1)
		error("failed to send letter '%c' (%d): %s", c, c, strerror(errno));
	if (msgrcv(queue_id, &msg, sizeof msg.kill_letter, letter_is_printed, 0) == -1)
		error("failed to receive answer for letter '%c' (%d): %s", c, c, strerror(errno));
}

void kill_letter(int c)
{
	if (letters[c]) {
		struct msgbuf_t msg;
		msg.mtype = letter_to_type(c);
		msg.kill_letter = true;
		if (msgsnd(queue_id, &msg, sizeof msg.kill_letter, 0) == -1)
			error("failed to kill letter '%c' (%d): %s", c, c, strerror(errno));
	}
}

int main(int argc, char *argv[])
{
	setvbuf(stdout, NULL, _IONBF, 0);

	if ((queue_id = msgget(IPC_PRIVATE, 0700)) == -1)
		error("failed to create queue");

	while (--argc > 0) {
		++argv;
		for (char *p = *argv; *p; ++p)
			send_letter(*p);
		send_letter((argc > 1) ? ' ' : '\n');
	}

	for (int c = 0; c < 256; ++c)
		kill_letter(c);
	for (int i = 0; i < nchildren; ++i)
		if (wait(NULL) == -1)
			perror("wait()");
	return 0;
}