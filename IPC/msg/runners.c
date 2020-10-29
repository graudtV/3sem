#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h> // fork
#include <sys/wait.h>
#include <assert.h>

#define FREQUENT_FEATURES_SINGLE_FILE
#include "../../frequent_features.h"

void snd_msg(int msgq_id, int participant_id) // кому
{
	long msg = participant_id + 1;
	assert(msg > 0);
	if (msgsnd(msgq_id, &msg, 0, 0) == -1)
		error("failed to send message: %s", strerror(errno));
}

void wait_msg(int msgq_id, int participant_id) // по своему id - чужие сообщения не читаем
{
	long msg;
	assert(participant_id >= 0);
	if (msgrcv(msgq_id, &msg, 0, participant_id + 1, 0))
		error("failed to recieve message: %s", strerror(errno));
}

// runner identifiers must be consequitive
void judge(int msgq_id, int participant_id, int first_runner_id, int last_runner_id)
{
	int nrunners = last_runner_id - first_runner_id + 1;
	printf("nrunners = %d\n", nrunners);
	printf("judge: на месте\n");

	printf("judge: жду игроков\n");
	// - wait all runners
	int narrived = 0;
	while (narrived < nrunners) {
		wait_msg(msgq_id, participant_id);
		++narrived;
	}
	printf("narrived = %d\n", narrived);
	printf("judge: все игроки пришли, начинаем игру\n");
	printf("judge: передаю палочку первому спортсмену\n");
	// - передать палочку первому
	snd_msg(msgq_id, first_runner_id);

	printf("judge: жду, когда они наконец закончат\n");
	// - ждать палочку
	wait_msg(msgq_id, participant_id);
	printf("judge: неужели, не прошло и полгода. Все прибежали\n");
	printf("judge: ну молодцы, теперь по домам\n");
	// - отпустить всех
	for (int i = first_runner_id; i <= last_runner_id; ++i)
		snd_msg(msgq_id, i);

	printf("judge: до свидания, я пошел\n");
}

void runner(int msgq_id, int participant_id, int next_participant_id, int judge_id)
{
	printf("runner #%d: на месте\n", participant_id);
	// - передать, что прибыл
	snd_msg(msgq_id, judge_id);

	printf("runner #%d: жду, когда можно будет бежать\n", participant_id);
	// - wait команды на свой id
	wait_msg(msgq_id, participant_id);
	printf("runner #%d: бегу\n", participant_id);
	printf("runner #%d: добежал, передаю палочку\n", participant_id);
	// - передать палочку другому
	snd_msg(msgq_id, next_participant_id);

	printf("runner #%d: жду, когда меня отпустят\n", participant_id);
	// - дождаться, пока судья отпустит
	wait_msg(msgq_id, participant_id);
	printf("runner #%d: до свидания, я пошел\n", participant_id);
}

int main()
{
	setvbuf(stdout, NULL, _IOLBF, BUFSIZ);
	int nrunners = 10000;
	int msgq_id = msgget(IPC_PRIVATE, 0600);
	
	PARALLEL(judge(msgq_id, 0, 1, nrunners));
	for (int i = 1; i < nrunners; ++i)
		PARALLEL(runner(msgq_id, i, i + 1, 0));
	PARALLEL(runner(msgq_id, nrunners, 0, 0))

	for (int i = 0; i < nrunners + 1; ++i) // runners + judge
		wait(NULL);

	if (msgctl(msgq_id, IPC_RMID, 0) == -1)
		error("failed to delete queue");

	return 0;
}