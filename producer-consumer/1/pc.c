#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/queue.h>

struct _Data {
	int 	value;
	TAILQ_ENTRY(_Data)	entry;
};

struct __workq {
	int							cnt;	/* 队列中元素个数 */
	pthread_mutex_t				mutex;
	pthread_cond_t				cond;
	TAILQ_HEAD(workq, _Data)	head;	/* 队列头 */
};

static void queue_add(struct __workq *pworkq)	/* 生产者：往队列首部插入元素 */
{
	struct _Data *pdata = (struct _Data *)calloc(1, sizeof(struct _Data));
	pdata->value = rand() % 1000 + 1;
	pthread_mutex_lock(&pworkq->mutex);
	TAILQ_INSERT_HEAD(&pworkq->head, pdata, entry);	
	++pworkq->cnt;
	printf("Produce %d(%d)\n", pdata->value, pworkq->cnt);
	pthread_mutex_unlock(&pworkq->mutex);
	pthread_cond_signal(&pworkq->cond);	/* 通知消费者 */
}

/* 生产者 */
void *producer(void *data)
{
	struct __workq *pworkq = (struct __workq *)data;

	while (1) {
		queue_add(pworkq);			
	}	
}

static void queue_remove(struct __workq *pworkq)	/* 消费者：删除首部元素 */
{
	pthread_mutex_lock(&pworkq->mutex);
	while (TAILQ_EMPTY(&pworkq->head)) {			/* 这里使用while是为了避免`惊群现象` */
		pthread_cond_wait(&pworkq->cond, &pworkq->mutex);	/* 队列为空，消费者等待 */
		printf("---------------------------->empty1.\n");
	}
	struct _Data *pdata = TAILQ_FIRST(&pworkq->head);
	TAILQ_REMOVE(&pworkq->head, pdata, entry);
	--pworkq->cnt;
	printf("Consume %d(%d)\n", pdata->value, pworkq->cnt);
	pthread_mutex_unlock(&pworkq->mutex);
	free(pdata);
}

/* 消费者 */
void *consumer(void *data)
{
	struct __workq *pworkq = (struct __workq *)data;

	while (1) {
		queue_remove(pworkq);
	}
}

int main(int argc, const char *argv[])
{
	pthread_t	pid, cid;
#if 0
	struct __workq	WorkQ = { 0, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, TAILQ_HEAD_INITIALIZER(WorkQ.head)};
#else
	struct __workq 	WorkQ;
	WorkQ.cnt = 0;
	pthread_mutex_init(&WorkQ.mutex, NULL);
	pthread_cond_init(&WorkQ.cond, NULL);
	TAILQ_INIT(&WorkQ.head);
#endif

	srand(time(NULL));	

	pthread_create(&pid, NULL, producer, &WorkQ);	/* 生产者 */
	pthread_create(&cid, NULL, consumer, &WorkQ);	/* 消费者 */

	pthread_join(pid, NULL);
	pthread_join(cid, NULL);
	pthread_mutex_destroy(&WorkQ.mutex);
	pthread_cond_destroy(&WorkQ.cond);

	exit(EXIT_SUCCESS);
}

/*
 *	关于`惊群现象(spurious wakeup)`的文章链接
 *		http://vladimir_prus.blogspot.com/2005/07/spurious-wakeups.html	
 *	总结如下:
 *		1. 	pthread_cond_wait()和pthread_cond_timedwait()函数的返回并不一定是由pthread_cond_signal()和pthread_cond_broadcast()唤醒
 *		   	的。这种现象叫做`假唤醒`，是POSIX明确允许的。因此，我们必须再次验证一下条件变量。
 *		2.  futex 不懂	
 */
