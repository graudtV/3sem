#include <stdio.h>
#include <pthread.h>

#define FREQUENT_FEATURES_SINGLE_FILE
#include "../frequent_features.h"

int count = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *dumb(void *p_niterations)
{
	int niterations = * (int *) p_niterations;
	printf("thread: %d\n", niterations);

	for (int i = 0; i < niterations; ++i)
		++count;
	return NULL;
}

void *naive(void *p_niterations)
{
	int niterations = * (int *) p_niterations;
	printf("thread: %d\n", niterations);

	for (int i = 0; i < niterations; ++i) {
		pthread_mutex_lock(&mutex);
		++count;
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}

void *wide(void *p_niterations)
{
	int niterations = * (int *) p_niterations;
	printf("thread: %d\n", niterations);

	pthread_mutex_lock(&mutex);
	for (int i = 0; i < niterations; ++i)
		++count;
	pthread_mutex_unlock(&mutex);
	return NULL;
}

void *quick(void *p_niterations)
{
	int niterations = * (int *) p_niterations;
	printf("thread: %d\n", niterations);

	int local_count = 0;
	for (int i = 0; i < niterations; ++i)
		++local_count;
	pthread_mutex_lock(&mutex);
	count += local_count;
	pthread_mutex_unlock(&mutex);

	return NULL;
}

void usage_error()
{
	fprintf(stderr, "usage: ./a.out N nthreads algorithm_type\n");
	fprintf(stderr, "\talgorithm_type:\td - dumb\n");
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	int N, nthreads;
	char algorithm_type;

	void *(*thread_routine)(void *) = dumb;

	if (argc != 4
		|| sscanf(argv[1], "%d", &N) != 1
		|| sscanf(argv[2], "%d", &nthreads) != 1
		|| sscanf(argv[3], "%c", &algorithm_type) != 1)
		usage_error();

	switch (algorithm_type) {
		case 'd': thread_routine = dumb; break;
		case 'n': thread_routine = naive; break;
		case 'w': thread_routine = wide; break;
		case 'q': thread_routine = quick; break;
		default:
			info(stderr, "incorrect algorithm_type");
			usage_error();
			break;
	}

	int count_per_thread = N / nthreads;
	int count_last_thread = count_per_thread + N % nthreads;
	pthread_t threads[nthreads];

	for (int i = 0; i < nthreads - 1; ++i)
		pthread_create(&threads[i], 0, thread_routine, &count_per_thread);
	pthread_create(&threads[nthreads - 1], 0, thread_routine, &count_last_thread);

	int res;
	for (int i = 0; i < nthreads; ++i)
		if ((res = pthread_join(threads[i], NULL)) != 0)
			error("pthread_join() error: %s", strerror(res));

	printf("count = %d\n", count);
	return 0;
}