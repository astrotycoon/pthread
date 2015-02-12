#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

/*
 * 简单的学习sem_init/sem_wait/sem_post的简单实用
 * 可以简单的认为	sem_init --> pthread_cond_init
 * 					sem_wait --> pthread_cond_wait	
 * 					sem_post --> pthread_cond_signal
 * */

#define PCHECK(CALL) do {												\
	int	result;															\
	if ((result = (CALL)) != 0) {										\
		fprintf(stderr, "FATAL: %s (%s)\n", strerror(result), #CALL);	\
		exit(EXIT_FAILURE);												\
	}																	\
} while (/*CONSTCOND*/0)

#define SEMCHECK(CALL) do {			\
	int result;						\
	if ((result = (CALL)) != 0) {	\
		fprintf(stderr, "FATAL: %s (%s)\n", strerror(errno), #CALL);	\
		exit(EXIT_FAILURE);			\
	}								\
} while (/*CONSTCOND*/0) 

sem_t	sem;

static void *thread_proc1(void *arg)
{
	printf("thread_proc1 ----------- sem_wait\n");
	sem_wait(&sem);
	printf("sem_wait\n");
	while (1);
}

static void *thread_proc2(void *arg)
{
	printf("thread_proc1 ----------- sem_post\n");
	sem_post(&sem);
	printf("sem_post\n");
	while (1);
}

int main(int argc, const char *argv[])
{
	pthread_t	thid[2];
	/* 初始化信号量 -- 线程间共享，初始值为0 */
	SEMCHECK(sem_init(&sem, 0, 0));
	printf("sem_init\n");

	PCHECK(pthread_create(&thid[0], NULL, thread_proc1, NULL));
	printf("thread_proc1\n");
	sleep(5);
	printf("sleep\n");
	PCHECK(pthread_create(&thid[1], NULL, thread_proc2, NULL));	
	printf("thread_proc2\n");

	PCHECK(pthread_join(thid[0], NULL));
	PCHECK(pthread_join(thid[1], NULL));
	sem_destroy(&sem);

	exit(EXIT_SUCCESS);
}
