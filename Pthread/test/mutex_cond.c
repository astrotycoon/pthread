#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>

#define THREAD_NUMBER	10

pthread_mutex_t mutex;
pthread_cond_t cond;
int running[THREAD_NUMBER] = {0};

int check_conflict(void);
void *thread_proc(void *arg);

int main(int argc, char *argv[])
{
	int ret, i;
	pthread_t thread[THREAD_NUMBER];
		
	ret = pthread_mutex_init(&mutex, NULL);
	assert(ret == 0);
	ret = pthread_cond_init(&cond, NULL);	
	assert(ret == 0);

	for (i = 0; i < THREAD_NUMBER; i++)
	{
		ret = pthread_create(&thread[i], NULL, thread_proc, (void *)i);
		assert(ret == 0);
	}
	
	for (i = 0; i < THREAD_NUMBER; i++)
	{
		ret = pthread_join(thread[i], NULL);
		assert(ret == 0);
	}

	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);

	exit(EXIT_SUCCESS);
}

int check_conflict(void)
{
	int i;
	int sum = 0;

	for (i = 0; i < THREAD_NUMBER; i++)
	{
		sum += running[i];
	}

	if (sum > 1)
		return 1;
	else
		return 0;
}

void *thread_proc(void *arg)
{
	int i = (int)arg;
	
	while (1)
	{
		pthread_mutex_lock(&mutex);		
//		pthread_cond_wait(&cond, &mutex);
		running[i] = 1;

		if (!check_conflict())
			printf("conflict error!\n");
		running[i] = 0;
//		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&mutex);
	}

	return NULL;
}
