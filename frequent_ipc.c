#include "frequent_ipc.h"
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdarg.h>

static int sem_array_v(key_t key, int nsems, int semflg, va_list list);


/*  Creates semaphore array and initialises semaphore_t structures passed
 * by ellipsis
 *  Parameters key and semflg will be passed to semget() to create
 * a semaphore set or to get identifier of existing one.
 *  Ellipsis parameters must be pointers of type semaphore_t * to existing
 * semaphore_t objects. The last pointer must be NULL (!). sem_array()
 * will create as many semaphores in one set as ellipsis parameters
 * were passed and will initialise pointed objects, so they could
 * be used in functions like sem_inc(), sem_dec() (if no error occured)
 *  Returns semaphore set identifier or -1 on error
 */
int sem_array(key_t key, int semflg, /* semaphore_t *psem1, */ ...)
{
	int nsems = 0;
	semaphore_t *psem;

	va_list ap;
	va_start(ap, semflg);
	while ((psem = va_arg(ap, semaphore_t *)) != NULL)
		++nsems;
	va_end(ap);

	va_start(ap, semflg);
	int ret = sem_array_v(key, nsems, semflg, ap);
	va_end(ap);
	return ret;
}

/* Destroys semaphore set */
int sem_destroy(int sem_array_id)
	{ return semctl(sem_array_id, 0, IPC_RMID); }

/*  Adds value to sem
 *  If argument sem was not initialized with sem_array(),
 * behaviour is undefined */
int sem_add_signed(semaphore_t sem, int value)
{
	struct sembuf op;
	op.sem_num = sem.sem_idx;
	op.sem_op = value;
	op.sem_flg = 0; // no flag
	return semop(sem.sem_array_id, &op, 1); // 1 - one operation
}

int sem_inc(semaphore_t sem)
	{ return sem_add_signed(sem, 1); }

int sem_dec(semaphore_t sem)
	{ return sem_add_signed(sem, -1); }

int sem_add(semaphore_t sem, unsigned int value)
	{ return sem_add_signed(sem, value); }

int sem_sub(semaphore_t sem, unsigned int value)
	{ return sem_add_signed(sem, -value); }

int sem_destroy_group(semaphore_t member)
	{ return sem_destroy(member.sem_array_id); }

/* helper function for sem_array() */
static int sem_array_v(key_t key, int nsems, int semflg, va_list list)
{
	// TODO: проинциализировать семафоры вручную для переносимости
	int sem_array_id = semget(key, nsems, semflg);
	if (sem_array_id != -1) {
		int i = 0;
		semaphore_t *psem;

		while (i < nsems && (psem = va_arg(list, semaphore_t *)) != NULL) {
			psem->sem_array_id = sem_array_id;
			psem->sem_idx = i;
			++i;
		}
	}
	return sem_array_id;
}
