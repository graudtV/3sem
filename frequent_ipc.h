/* Usefull IPC functions */
/* Functions' descriptions could be found in frequent_ipc.c */
#ifndef FREQUENT_IPC_H_
#define FREQUENT_IPC_H_

#include <sys/types.h> // for key_t
#include <stdio.h>

/*  Describes individual semaphore in System V semaphore set
 *  Structure is used in functions like sem_inc(), sem_dec()
 * to conviniently alter individual semaphores values. An object
 * of this structure can be safely passed into functions */
typedef struct {
	int sem_array_id; // semaphore set identifier
	int sem_idx; // semaphore number in the set
} semaphore_t;

int sem_array(key_t key, int semflg, /* semaphore_t *psem1, */ ...);
int sem_destroy(int sem_array_id);
int sem_destroy_group(semaphore_t member);

int sem_inc(semaphore_t sem);
int sem_dec(semaphore_t sem);
int sem_add(semaphore_t sem, unsigned int value);
int sem_sub(semaphore_t sem, unsigned int value);
int sem_add_signed(semaphore_t sem, int value);

/*  There is no setlinebuf() in stdio.h according to standard.
 * But Linux has it. So it is defined as a macro to avoid conflicts */
#define setlinebuf(stream) setvbuf((stream), NULL, _IOLBF, 0)

/*  If macro FREQUENT_FEATURES_SINGLE_FILE is defined,
 * source code of frequent_ipc.c will be included in place */
#ifdef FREQUENT_FEATURES_SINGLE_FILE
#include "frequent_ipc.c"
#endif

#endif // FREQUENT_IPC_H_