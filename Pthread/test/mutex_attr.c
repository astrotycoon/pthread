#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#ifndef _POSIX_THREAD_PROCESS_SHARED
#   error "This platform does not support process shared mutex"
#endif

/* Define globally accessible variables and a mutex */

#define NUMTHRDS 4

int *shared_sum = NULL;

pthread_mutex_t *sum_mutex;

int process_fn()
{
	int i;
	for (i = 0; i < 100000000; i++)
	{
		pthread_mutex_lock(sum_mutex);
		*shared_sum += 1;
		pthread_mutex_unlock(sum_mutex);
	}
	return 0;
}

int main(int argc, char **argv)
{

	int i;
	pthread_mutex_t *p_map;
	int *temp;
	int cpid;
	p_map = (pthread_mutex_t *) mmap(NULL, sizeof(pthread_mutex_t) * 10, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	sum_mutex = p_map;
	shared_sum = (int *) (p_map + 3);
	*shared_sum = 0;

	pthread_mutexattr_t mutex_shared_attr;

	/* Set pthread_mutex_attr to process shared */
	pthread_mutexattr_init(&mutex_shared_attr);
	pthread_mutexattr_setpshared(&mutex_shared_attr, PTHREAD_PROCESS_SHARED);

	pthread_mutex_init(sum_mutex, &mutex_shared_attr);

	cpid = fork();
	if (cpid == 0)
	{
		process_fn();
		return 0;
	}
	else
	{
		process_fn();
		waitpid(cpid, NULL, 0);
	}

	temp = (int *) (p_map + 3);
	printf("in the end, the shared sum: %d, %d\n", *temp, *shared_sum);
	return 0;
}
