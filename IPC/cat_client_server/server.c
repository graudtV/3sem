#include "common.h"
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <signal.h>

#define FREQUENT_FEATURES_SINGLE_FILE
#include "../../frequent_features.h"
#include "../../frequent_ipc.h"

semaphore_t buffer_can_read;
semaphore_t buffer_can_write;
int shm_id;
char *shared_buf;

void cleanup(int unused)
{
	fprintf(stderr, "----- server shut down -----\n");
	if (sem_destroy_group(buffer_can_read) == -1)
		error("failed to destroy semaphores");
	if (shmctl(shm_id, IPC_RMID, NULL) == -1)
		error("failed to destroy shared memory");
	exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
	key_t key = ftok(argv[0], 1);
	PROGRAM_NAME = argv[0];
	signal(SIGINT, cleanup);

	if (sem_array(key, 0600 | IPC_CREAT | IPC_EXCL,
			&buffer_can_read, &buffer_can_write, NULL) == -1)
		error("failed to create semaphores: %s", strerror(errno));
	sem_inc(buffer_can_write);

	// Shared mem [MAXBUF]
	if ((shm_id = shmget(key, MAXBUF, 0600 | IPC_CREAT | IPC_EXCL)) == -1
		|| (shared_buf = shmat(shm_id, NULL, SHM_RDONLY)) == (void *) (-1))
		error("failed to create shared memory: %s", strerror(errno));

	fprintf(stderr, "----- server started (waiting for clients) -----\n");

	while (1) {
		struct buffer_t local_buf;

		sem_dec(buffer_can_read);
		strncpy(local_buf.data, shared_buf, MAXBUF); // create local copy
		sem_inc(buffer_can_write);
		
		write(STDOUT_FILENO, local_buf.data, strlen(local_buf.data)); // copy local_buf to stdout
	}

	cleanup(0);

	return 0;
}