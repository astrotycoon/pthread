#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int i = 1;

void *thread1(void *);
void *thread2(void *);

int main(int argc, const char *argv[])
{
	int ret;
	pthread_t tid1, tid2;	

	ret = pthread_create(&tid1, NULL, thread1, NULL);
	assert(ret == 0);
	ret = pthread_create(&tid2, NULL, thread2, NULL);
	assert(ret == 0);

	ret = pthread_join(tid1, NULL);
	assert(ret == 0);
	ret = pthread_join(tid2, NULL);
	assert(ret == 0);
	
	return 0;
}

void *thread1(void *arg)
{
	for (i = 1; i <= 9; i++)
	{
		pthread_mutex_lock(&mutex);
		if ((i % 3) == 0)
		{
			pthread_mutex_unlock(&mutex);
			pthread_cond_signal(&cond);
		}
		else
		{
			printf("thread1: %d\n", i);
			pthread_mutex_unlock(&mutex);
		}
		sleep(1);
	}
}

void *thread2(void *arg)
{
	while (i < 9)
	{
		pthread_mutex_lock(&mutex);
		while ((i % 3) != 0)	// 其实是等待i % 3 == 0
		{
			pthread_cond_wait(&cond, &mutex);
		}
		printf("thread2: %d\n", i);
		pthread_mutex_unlock(&mutex);
		sleep(2);
	}
}
