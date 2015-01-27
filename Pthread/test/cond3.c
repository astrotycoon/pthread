#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int volatile iCount = 0;

void *increament_proc(void *arg)
{
	while (1)
	{
		pthread_mutex_lock(&mutex);
		iCount++;	
		pthread_mutex_unlock(&mutex);

		if (iCount >= 100)	
		{
			pthread_cond_signal(&cond);
		}
	}
}

void *decreament_proc(void *arg)
{
	while (1)
	{
		pthread_mutex_lock(&mutex);
		iCount--;	
		pthread_mutex_unlock(&mutex);
	}
}

void *infomation_proc(void *arg)
{
	while (1)
	{
		pthread_mutex_lock(&mutex);
		while (iCount < 100)
		{
			pthread_cond_wait(&cond, &mutex);
		}
		printf("iCount >= 100\n");
		iCount = 0;
		pthread_mutex_unlock(&mutex);
	}
}

int main(int argc, const char *argv[])
{
	int ret;
	pthread_t tid1, tid2, tid3, tid4;
	
	ret = pthread_create(&tid1, NULL, increament_proc, NULL);
	assert(ret == 0);
	ret = pthread_create(&tid2, NULL, increament_proc, NULL);
	assert(ret == 0);
	ret = pthread_create(&tid3, NULL, decreament_proc, NULL);
	assert(ret == 0);
	ret = pthread_create(&tid4, NULL, infomation_proc, NULL);
	assert(ret == 0);
		
	ret = pthread_join(tid1, NULL);
	assert(ret == 0);
	ret = pthread_join(tid2, NULL);
	assert(ret == 0);
	ret = pthread_join(tid3, NULL);
	assert(ret == 0);
	ret = pthread_join(tid4, NULL);
	assert(ret == 0);
	return 0;
}
