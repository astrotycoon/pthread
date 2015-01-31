#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>
#include "apr/apr_pools.h"
#include "apr/apr_thread_proc.h"
#include "apr/apr_thread_mutex.h"
#include "apr/apr_thread_cond.h"
#include "apr/apr_errno.h"

struct _Data {
	int 	value;
	TAILQ_ENTRY(_Data)	entry;
};

struct __workq {
	int							cnt;	/* 队列中元素个数 */
	apr_thread_mutex_t			*mutex;
	apr_thread_cond_t			*cond;
	TAILQ_HEAD(workq, _Data)	head;	/* 队列头 */
};

static void queue_add(struct __workq *pworkq)	/* 生产者：往队列首部插入元素 */
{
	struct _Data *pdata = (struct _Data *)calloc(1, sizeof(struct _Data));
	pdata->value = rand() % 1000 + 1;
	apr_thread_mutex_lock(pworkq->mutex);
	TAILQ_INSERT_HEAD(&pworkq->head, pdata, entry);	
	++pworkq->cnt;
	printf("Produce %d(%d)\n", pdata->value, pworkq->cnt);
	apr_thread_mutex_unlock(pworkq->mutex);
	apr_thread_cond_signal(pworkq->cond);	/* 通知消费者 */
}

/* 生产者 */
void *producer(apr_thread_t *th, void *data)
{
	struct __workq *pworkq = (struct __workq *)data;

	while (1) {
		queue_add(pworkq);			
	}	
}

static void queue_remove(struct __workq *pworkq)	/* 消费者：删除首部元素 */
{
	apr_thread_mutex_lock(pworkq->mutex);
	while (TAILQ_EMPTY(&pworkq->head)) {			/* 这里使用while是为了避免`惊群现象` */
		apr_thread_cond_wait(pworkq->cond, pworkq->mutex);	/* 队列为空，消费者等待 */

		printf("---------------------------->empty1.\n");
	}
	struct _Data *pdata = TAILQ_FIRST(&pworkq->head);
	TAILQ_REMOVE(&pworkq->head, pdata, entry);
	--pworkq->cnt;
	printf("Consume %d(%d)\n", pdata->value, pworkq->cnt);
	apr_thread_mutex_unlock(pworkq->mutex);
	free(pdata);
}

/* 消费者 */
void *consumer(apr_thread_t *th, void *data)
{
	struct __workq *pworkq = (struct __workq *)data;

	while (1) {
		queue_remove(pworkq);
	}
}

int main(int argc, const char *argv[])
{
	apr_status_t	status;
	apr_pool_t		*pool;
	apr_thread_t	*pth, *cth;
	char			errmsg[256];

	struct __workq	WorkQ;	
	
begin:
	/* 初始化pool */
	status = apr_pool_initialize();	
	if (status != APR_SUCCESS) {
		fprintf(stderr, "%s\n", apr_strerror(status, errmsg, sizeof(errmsg)));	
		exit(EXIT_FAILURE);
	}
	/* 创建一个pool*/
	status = apr_pool_create(&pool, NULL);
	if (status != APR_SUCCESS) {
		fprintf(stderr, "%s\n", apr_strerror(status, errmsg, sizeof(errmsg)));	
		apr_pool_terminate();
		exit(EXIT_FAILURE);
	}
	/* 初始化队列 */
	WorkQ.cnt = 0;
	TAILQ_INIT(&WorkQ.head);
	status = apr_thread_mutex_create(&WorkQ.mutex, APR_THREAD_MUTEX_DEFAULT, pool);
	if (status != APR_SUCCESS) {
		fprintf(stderr, "%s\n", apr_strerror(status, errmsg, sizeof(errmsg)));	
		apr_pool_destroy(pool);
		apr_pool_terminate();
		exit(EXIT_FAILURE);
	}
	status = apr_thread_cond_create(&WorkQ.cond, pool);
	if (status != APR_SUCCESS) {
		fprintf(stderr, "%s\n", apr_strerror(status, errmsg, sizeof(errmsg)));	
		apr_thread_mutex_destroy(WorkQ.mutex);
		apr_pool_destroy(pool);
		apr_pool_terminate();
		exit(EXIT_FAILURE);
	}
	/* 创建生产者线程 */
	status = apr_thread_create(&pth, NULL, producer, &WorkQ, pool);
	if (status != APR_SUCCESS) {
		fprintf(stderr, "%s\n", apr_strerror(status, errmsg, sizeof(errmsg)));	
		apr_thread_cond_destroy(WorkQ.cond);	
		apr_thread_mutex_destroy(WorkQ.mutex);
		apr_pool_destroy(pool);
		apr_pool_terminate();
		exit(EXIT_FAILURE);
	}
	/* 创建消费者线程 */
	status = apr_thread_create(&cth, NULL, consumer, &WorkQ, pool);
	if (status != APR_SUCCESS) {
		fprintf(stderr, "%s\n", apr_strerror(status, errmsg, sizeof(errmsg)));	
		apr_thread_cond_destroy(WorkQ.cond);	
		apr_thread_mutex_destroy(WorkQ.mutex);
		apr_pool_destroy(pool);
		apr_pool_terminate();
		exit(EXIT_FAILURE);
	}

	apr_thread_join(NULL, pth);
	apr_thread_join(NULL, cth);

end:
	apr_thread_cond_destroy(WorkQ.cond);	
	apr_thread_mutex_destroy(WorkQ.mutex);
	apr_pool_destroy(pool);
	apr_pool_terminate();
	exit(EXIT_SUCCESS);
}
