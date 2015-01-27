#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

volatile int count = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *A(void *arg)
{
	int ret;
	pthread_mutex_lock(&mutex);
	while (count == 0)
		ret = pthread_cond_wait(&cond, &mutex);
	printf("decrement: ret = %d, count = %d, %s\n",
						ret, count, strerror(ret));
	count--;
	printf("decrement: count = %d\n", count);
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
}

void *B(void *arg)
{
	int ret;
	
	pthread_mutex_lock(&mutex);
	sleep(3);
	count++;
	pthread_mutex_unlock(&mutex);
	pthread_cond_signal(&cond);
	printf("increment: count = %d\n", count);
	pthread_exit(NULL);
}

int main(int argc, const char *argv[])
{
	int ret;
	pthread_t tid1, tid2;

	ret = pthread_create(&tid1, NULL, A, NULL);
	assert(ret == 0);
	ret = pthread_create(&tid2, NULL, B, NULL);
	assert(ret == 0);

	ret = pthread_join(tid1, NULL);
	assert(ret == 0);
	ret = pthread_join(tid2, NULL);
	assert(ret == 0);
	
	return 0;
}
