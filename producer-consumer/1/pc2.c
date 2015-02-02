#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <sys/queue.h>

#define PCHECK(CALL) do {												\
	int	result;															\
	if ((result = (CALL)) != 0) {										\
		fprintf(stderr, "FATAL: %s (%s)\n", strerror(result), #CALL);	\
		exit(EXIT_FAILURE);												\
	}																	\
} while (/*CONSTCOND*/0)

#define QUEUE_MAX_NUM		(1000)	/* 规定队列最多拥有1000个队列元素 */
#define PRODUCER_THREAD_NUM	(5)		/* 生产者线程个数 */
#define CONSUMER_THREAD_NUM	(3)		/* 消费者线程个数 */

/* 队列元素 -- 这里为了简单，只有一个int型整数 */
struct _Data {
	int 	value;
	TAILQ_ENTRY(_Data)	entry;
};

struct __workq {
	TAILQ_HEAD(workq, _Data)	head;		/* 队列头 */
	int							cnt;		/* 队列中元素个数 */
	pthread_mutex_t				lock;		/* 用于保护队列和两个条件变量 */
	pthread_cond_t				got_consumer_cond;	/* 用于队列由满变为非满时唤醒生产者继续生产 */
	pthread_cond_t				got_producer_cond;	/* 用于队列由空变为非空时唤醒消费者继续消费 */
};

static void queue_add(struct __workq *pworkq)
{
	PCHECK(pthread_mutex_lock(&pworkq->lock));
	while (pworkq->cnt == QUEUE_MAX_NUM) {	/* 队列已满 */
		printf("---------------------------->full.\n");
		/* 那么生产者休眠，等待消费者消费之后的通知  */
		PCHECK(pthread_cond_wait(&pworkq->got_consumer_cond, &pworkq->lock));
	}
	struct _Data *pdata = (struct _Data *)calloc(1, sizeof(struct _Data));
	pdata->value = rand() % 1000 + 1;
	TAILQ_INSERT_HEAD(&pworkq->head, pdata, entry);	
	++pworkq->cnt;
	printf("Produce %d(%d)\n", pdata->value, pworkq->cnt);
	PCHECK(pthread_cond_signal(&pworkq->got_producer_cond));	/* 通知消费者可以继续消费了 */
	PCHECK(pthread_mutex_unlock(&pworkq->lock));
//	PCHECK(pthread_cond_signal(&pworkq->got_producer_cond));	/* 通知消费者可以继续消费了 */
}

static void *producer(void *args)
{
	struct __workq	*pworkq = (struct __workq *)args;

	while (1) {
		queue_add(pworkq);			
	}
}

static void queue_remove(struct __workq *pworkq)
{
	PCHECK(pthread_mutex_lock(&pworkq->lock));
	while (pworkq->cnt == 0) {	/* 队列已空 */
		printf("---------------------------->empty.\n");
		/* 那么消费者休眠，等待生产者生产之后的通知 */
		PCHECK(pthread_cond_wait(&pworkq->got_producer_cond, &pworkq->lock));
	}
	struct _Data *pdata = TAILQ_FIRST(&pworkq->head);
	TAILQ_REMOVE(&pworkq->head, pdata, entry);
	--pworkq->cnt;
	printf("Consume %d(%d)\n", pdata->value, pworkq->cnt);
	PCHECK(pthread_cond_signal(&pworkq->got_consumer_cond));	/* 通知生产者可以继续生产 */
	PCHECK(pthread_mutex_unlock(&pworkq->lock));
//	PCHECK(pthread_cond_signal(&pworkq->got_consumer_cond));	/* 通知生产者可以继续生产 */
	free(pdata);
}

static void *consumer(void *args)
{
	struct __workq	*pworkq = (struct __workq *)args;

	while (1) {
		queue_remove(pworkq);
	}
}

int main(int argc, const char *argv[])
{
	pthread_t		pth[PRODUCER_THREAD_NUM];
	pthread_t		cth[CONSUMER_THREAD_NUM];
	struct __workq 	WorkQ; 
	int 			i;
begin:
	srand(time(NULL));
	WorkQ.cnt = 0;
	PCHECK(pthread_mutex_init(&WorkQ.lock, NULL));	
	PCHECK(pthread_cond_init(&WorkQ.got_consumer_cond, NULL));
	PCHECK(pthread_cond_init(&WorkQ.got_producer_cond, NULL));
	TAILQ_INIT(&WorkQ.head);

	/* 消费者线程 */
	for (i = 0; i < CONSUMER_THREAD_NUM; i++) {
		PCHECK(pthread_create(&cth[i], NULL, consumer, &WorkQ));
	}
	sleep(2);
	/* 生产者线程 */
	for (i = 0; i < PRODUCER_THREAD_NUM; i++) {
		PCHECK(pthread_create(&pth[i], NULL, producer, &WorkQ));
	}

end:
	for (i = 0; i < PRODUCER_THREAD_NUM; i++) {
		PCHECK(pthread_join(pth[i], NULL));
	}
	for (i = 0; i < CONSUMER_THREAD_NUM; i++) {
		PCHECK(pthread_join(cth[i], NULL));
	}

	PCHECK(pthread_mutex_destroy(&WorkQ.lock));
	PCHECK(pthread_cond_destroy(&WorkQ.got_consumer_cond));
	PCHECK(pthread_cond_destroy(&WorkQ.got_producer_cond));

	exit(EXIT_SUCCESS);
}
