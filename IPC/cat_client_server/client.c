#include "common.h"
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define FREQUENT_FEATURES_SINGLE_FILE
#include "../../frequent_features.h"
#include "../../frequent_ipc.h"

semaphore_t buffer_can_read;
semaphore_t buffer_can_write;
char *shared_buf;

void transmit_to_server(FILE *fin)
{
	struct buffer_t local_buf;
	while (fgets(local_buf.data, sizeof local_buf.data, fin) != NULL) { // read a line from stdin to local_buf
		if (sem_dec(buffer_can_write) == -1)
			error("connection lost: server unavailable");
		strncpy(shared_buf, local_buf.data, MAXBUF);
		if ((sem_inc(buffer_can_read)) == -1)
			error("connection lost: server unavailable");
	}
}


int main(int argc, char *argv[])
{
	key_t key = ftok("server", 1);
	PROGRAM_NAME = argv[0];

	if (sem_array(key, 0600, &buffer_can_read, &buffer_can_write, NULL) == -1)
		error("server unavailable");

	// Shared mem [MAXBUF]
	int shm_id;
	if ((shm_id = shmget(key, MAXBUF, 0600)) == -1
		|| (shared_buf = shmat(shm_id, NULL, 0)) == (void *) (-1))
		error("server unavailable");

	if (argc == 1)
		transmit_to_server(stdin);
	else
		while (--argc > 0) {
			FILE *fin = fopen(*++argv, "r");
			if (!fin) {
				info(stderr, "cannot open file '%s': %s", *argv, strerror(errno));
				continue;
			}
			transmit_to_server(fin);
			fclose(fin);
		}

	return 0;
}