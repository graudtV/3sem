#include <stdio.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <signal.h>

#define FREQUENT_FEATURES_SINGLE_FILE
#include "../frequent_features.h"
#include "../frequent_ipc.h"

semaphore_t bridge_command_car;
semaphore_t bridge_command_ship;
semaphore_t bridge_decide;
semaphore_t shared_data_mutex;
semaphore_t smb_waiting;

struct {
	int nships_awaiting;
	int ncars_awaiting;
	enum { BRIDGE_CLOSED = 0, BRIDGE_OPEN } bridge_state;
} *shared;

int shmid;

void car(int idx)
{
	sem_dec(shared_data_mutex);
	{ // critical section
		if (shared->nships_awaiting == 0 && shared->ncars_awaiting == 0)
			sem_inc(smb_waiting);
		shared->ncars_awaiting++;
	} // end of critical section
	sem_inc(shared_data_mutex);

	printf("car  #%2d: жду, когда можно будет заехать на мост\n", idx);
	sem_dec(bridge_command_car);
	printf("car  #%2d: еду по мосту\n", idx);
	printf("car  #%2d: уехал с моста\n", idx);
	sem_inc(bridge_decide);
}

void ship(int idx)
{
	sem_dec(shared_data_mutex);
	{ // critical section
		if (shared->nships_awaiting == 0 && shared->ncars_awaiting == 0)
			sem_inc(smb_waiting);
		shared->nships_awaiting++;
	} // end of critical section
	sem_inc(shared_data_mutex);

	printf("ship #%2d: жду, когда можно будет проплыть\n", idx);
	sem_dec(bridge_command_ship);
	printf("ship #%2d: плыву\n", idx);
	printf("ship #%2d: проплыл\n", idx);
	sem_inc(bridge_decide);
}

void bridge_cycle()
{
	enum { CAR, SHIP } who;

	sem_dec(bridge_decide);
	printf("bridge: мост свободен\n");

	printf("bridge: проверяю, есть ли ожидающая машина/корабль\n");
	sem_dec(smb_waiting);
	printf("bridge: желающий проехать/проплыть найден\n");

	sem_dec(shared_data_mutex);
	{ // critical section
		printf("bridge: ждет %d  машин и %d кораблей\n",
			shared->ncars_awaiting, shared->nships_awaiting);

		if (shared->ncars_awaiting > 0
			&& (shared->nships_awaiting == 0
				|| (shared->nships_awaiting == 1
					&& shared->bridge_state == BRIDGE_CLOSED))) {
			printf("bridge: пропускаю машину\n");
			who = CAR;
			shared->bridge_state = BRIDGE_CLOSED;
			shared->ncars_awaiting--;
		} else {
			printf("bridge: пропускаю корабль\n");
			who = SHIP;
			shared->bridge_state = BRIDGE_OPEN;
			shared->nships_awaiting--;
		}
		if (shared->nships_awaiting > 0 || shared->ncars_awaiting > 0)
			sem_inc(smb_waiting);
	} // end of critical section
	sem_inc(shared_data_mutex);
	
	if (who == CAR)
		sem_inc(bridge_command_car);
	else
		sem_inc(bridge_command_ship);

	printf("bridge: жду, когда мост освободится\n");
}

void bridge()
{
	while (1)
		bridge_cycle();
}

void cleanup(int unused)
{
	while (wait(NULL) != -1)
		;
	if (sem_destroy_group(bridge_command_car) == -1)
		error("failed to destroy semaphores: %s", strerror(errno));
	if (shmdt(shared) == -1 || shmctl(shmid, IPC_RMID, 0) == -1)
		error("failed to destroy shared memory: %s", strerror(errno));
	exit(EXIT_SUCCESS);
}

void init_ipcs()
{
	/* Disable SIGINT because it can make IPC data inconsistent */
	signal(SIGINT, SIG_IGN);

	if (sem_array(IPC_PRIVATE, 0600, &bridge_command_car, &bridge_command_ship,
		&bridge_decide, &shared_data_mutex, &smb_waiting, NULL) == -1)
		error("failed to create semaphores: %s", strerror(errno));
	sem_inc(bridge_decide);
	sem_inc(shared_data_mutex);

	if ((shmid = shmget(IPC_PRIVATE, sizeof (*shared), 0600)) == -1
		|| (shared = shmat(shmid, NULL, 0)) == NULL)
		error("failed to create shared memory: %s", strerror(errno));

	/* shared->bridge_state is BRIDGE_CLOSED by default, because
	 * BRIDGE_STATE = 0 and shmget returns memory, filled with nulls */

	signal(SIGINT, SIG_DFL);
}

int main(int argc, char *argv[])
{
	setlinebuf(stdout);
	setlinebuf(stderr);

	int ncars, nships;
	if (argc != 3
		|| sscanf(argv[1], "%d", &ncars) != 1
		|| sscanf(argv[2], "%d", &nships) != 1)
		error("usage: %s ncars nships", argv[0]);

	init_ipcs();

	for (int i = 0; i < nships; ++i)
		PARALLEL(ship(i + 1));
	for (int i = 0; i < ncars; ++i)
		PARALLEL(car(i + 1));
	PARALLEL(bridge());

	signal(SIGINT, cleanup);
	cleanup(0);
	return EXIT_SUCCESS;
}