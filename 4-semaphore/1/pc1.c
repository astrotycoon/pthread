#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/queue.h>

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

/* 队列元素 -- 这里为了简单，只有一个int型整数 */
struct _Data {
	int 	value;
	TAILQ_ENTRY(_Data)	entry;
};

#define QUEUE_MAX_NUM		(100000)	/* 规定队列最多拥有1000个队列元素 */
#define PRODUCER_THREAD_NUM	(5)		/* 生产者线程个数 */
#define CONSUMER_THREAD_NUM	(3)		/* 消费者线程个数 */

struct __workq {
	TAILQ_HEAD(workq, _Data)	head;		/* 队列头 */
	uint32_t					cnt;
	sem_t						lock;		/* 互斥信号量，用于互斥访问队列 */
	sem_t						empty;		/* 计数信号量，代表队列中空闲的元素个数 */	
	sem_t						full;		/* 计数信号量，代表队列中存在的元素个数 */	
};

static void queue_add(struct __workq *pworkq) 
{
	/*
 	 *	如果empty为0，表明队列已满，等待
 	 *	否则的话，empty减1
 	 */
	SEMCHECK(sem_wait(&pworkq->empty));

	SEMCHECK(sem_wait(&pworkq->lock));	/* 队列加锁 */

	struct _Data *pdate = (struct _Data *)calloc(1, sizeof(struct _Data));
	pdate->value = rand() % 1000 + 1;
	TAILQ_INSERT_HEAD(&pworkq->head, pdate, entry);	
	++pworkq->cnt;
	printf("['%lu' Produce %d] current cnt (%d)\n", pthread_self(), pdate->value, pworkq->cnt);

	SEMCHECK(sem_post(&pworkq->lock));	/* 队列解锁 */

	SEMCHECK(sem_post(&pworkq->full));	/* 队列元素增加1 */
}

static void *producer(void *args)
{
	struct __workq	*pworkq	= (struct __workq *)args;

	while (1) {
		queue_add(pworkq);	
	}
}

static void queue_remove(struct __workq *pworkq)
{
	/* 
 	 *	如果full为0，则证明队列为空，则等待
 	 *	否则的话，full减1
 	 */
	SEMCHECK(sem_wait(&pworkq->full));

	SEMCHECK(sem_wait(&pworkq->lock));	/* 队列加锁 */

	struct _Data *pdata = TAILQ_FIRST(&pworkq->head);
	TAILQ_REMOVE(&pworkq->head, pdata, entry);
	--pworkq->cnt;
	printf("['%lu' Consume %d] current cnt (%d)\n", pthread_self(), pdata->value, pworkq->cnt);
	free(pdata);

	SEMCHECK(sem_post(&pworkq->lock));	/* 队列解锁 */

	SEMCHECK(sem_post(&pworkq->empty));	/* 队列空闲元素增加1 */
}

static void *consumer(void *args)
{
	struct __workq	*pworkq	= (struct __workq *)args;

	while (1) {
		queue_remove(pworkq);
	}
}

int main(int argc, const char *argv[])
{
	pthread_t		ptid[PRODUCER_THREAD_NUM];	
	pthread_t		ctid[CONSUMER_THREAD_NUM];
	struct __workq	workq;
	int 			i;

begin:
	srand(time(NULL));
	workq.cnt = 0;
	TAILQ_INIT(&workq.head);
	/* 初始化互斥信号量为1 */
	SEMCHECK(sem_init(&workq.lock, 0, 1));
	/* 初始化计数信号量empty为QUEUE_MAX_NUM，说明队列为空 */
	SEMCHECK(sem_init(&workq.empty, 0, QUEUE_MAX_NUM));	
	/* 初始化计数信号量full为0，说明队列为空 */
	SEMCHECK(sem_init(&workq.full, 0, 0));	

	/* 消费者线程 */
	for (i = 0; i < CONSUMER_THREAD_NUM; i++) {
		PCHECK(pthread_create(&ctid[i], NULL, consumer, &workq));
	}
	sleep(3);
	/* 生产者线程 */
	for (i = 0; i < PRODUCER_THREAD_NUM; i++) {
		PCHECK(pthread_create(&ptid[i], NULL, producer, &workq));
	}
end:
	for (i = 0; i < PRODUCER_THREAD_NUM; i++) {
		PCHECK(pthread_join(ptid[i], NULL));
	}
	for (i = 0; i < CONSUMER_THREAD_NUM; i++) {
		PCHECK(pthread_join(ctid[i], NULL));
	}

	SEMCHECK(sem_destroy(&workq.lock));
	SEMCHECK(sem_destroy(&workq.empty));
	SEMCHECK(sem_destroy(&workq.full));

	exit(EXIT_SUCCESS);
}
