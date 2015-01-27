#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

int count = 0;
pthread_mutex_t mutex;
pthread_cond_t cond;

void *watch_count(void *);
void *inc_count(void *);

int main(int argc, const char *argv[])
{
	int i;
	long t1 = 1, t2 = 2, t3 = 3;
	pthread_t threads[3];
	pthread_attr_t attr;

	/* initialize mutex and condition variable objects */
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);
	/* for portability, explicitly create threads in a joinable state */
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_create(&threads[0], &attr, watch_count, (void *)t1);
	pthread_create(&threads[1], &attr, inc_count, (void *)t2);
	pthread_create(&threads[2], &attr, inc_count, (void *)t3);

	/* wait for all threads to complete */	
	for (i = 0; i < 3; i++)
	{
		pthread_join(threads[i], NULL);
	}
	printf("main(): wait on 3 threads. Done.\n");
	
	/* clean up and exit */
	pthread_attr_destroy(&attr);
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);
	pthread_exit(NULL);
}

void *watch_count(void *arg)
{
	long my_id = (long)arg;	

	printf("starting watch_count(): thread %ld\n", my_id);
	pthread_mutex_lock(&mutex);
	while (count < 12)
	{
		pthread_cond_wait(&cond, &mutex);
		printf("watch_count(): thread %ld condition signal received.\n", my_id);
		count += 125;
		printf("watch_count(): thread %ld count now = %d.\n", my_id, count);
	}
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
}

void *inc_count(void *arg)
{
	int i;
	long my_id = (long)arg;

	for (i = 0; i < 10; i++)	
	{
		pthread_mutex_lock(&mutex);
		count++;
		
		if (count == 12)
		{
			pthread_cond_signal(&cond);
			printf("inc_count(): thread %ld, count = %d,  cond reached.\n",
									my_id, count);
		}
		printf("inc_count(): thread %ld, count = %d, unlock mutex\n", 
									my_id, count);
		pthread_mutex_unlock(&mutex);
		sleep(1);
	}
	pthread_exit(NULL);
}
