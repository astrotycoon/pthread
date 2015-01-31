#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "queue.h"
#include "pjlib.h"

struct _Data {
	int 				value;
	TAILQ_ENTRY(_Data)	entry;
};

struct __workq {
	TAILQ_HEAD(workq, _Data)	head;	/* 队列首部 	*/
	uint32_t 					cnt;	/* 队列元素个数 */
	pj_mutex_t					*mutex;
	pj_event_t					*event;
};

static void queue_add(struct __workq *pworkq)	/* 生产者：往队列首部插入元素 */
{
	struct _Data *pdata = (struct _Data *)calloc(1, sizeof(struct _Data));
	pdata->value = rand() % 1000 + 1;

pj_mutex_lock(pworkq->mutex);
	TAILQ_INSERT_HEAD(&pworkq->head, pdata, entry);	
	++pworkq->cnt;
	printf("Produce %d(%u)\n", pdata->value, pworkq->cnt);
pj_mutex_unlock(pworkq->mutex);
	pj_event_set(pworkq->event);		/* 通知消费者 */
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
	while (pworkq->head.tqh_first == NULL) {
		printf("---------------------------->empty1.\n");
		pj_event_wait(pworkq->event);
	}

pj_mutex_lock(pworkq->mutex);
	struct _Data *pdata = pworkq->head.tqh_first;
	TAILQ_REMOVE(&pworkq->head, pdata, entry);
	--pworkq->cnt;
	printf("Consume %d(%u)\n", pdata->value, pworkq->cnt);
pj_mutex_unlock(pworkq->mutex);
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
	struct __workq	WorkQ = { 0 };;	
	char 			errmsg[PJ_ERR_MSG_SIZE];	
	pj_status_t		status;
	pj_caching_pool	caching_pool;
	pj_pool_t		*pool;
	pj_thread_t		*pth;	/* 生产者线程句柄 */
	pj_thread_t		*cth;	/* 消费者线程句柄 */

begin:
	srand(time(NULL));
	/* 初始化队列 */
	TAILQ_INIT(&WorkQ.head);
	/* 初始化pjlib库 */
	status = pj_init();
	pj_assert(status == PJ_SUCCESS);
	/* 创建内存池工厂 */	
	pj_caching_pool_init(&caching_pool, NULL, 1024 * 1024);
	/* 创建内存池	*/
	pool = pj_pool_create(&caching_pool.factory, NULL, 4096, 4096, NULL);
	pj_assert(status == PJ_SUCCESS);
	/* 初始化mutex */
	status = pj_mutex_create_simple(pool, NULL, &WorkQ.mutex);
	pj_assert(status == PJ_SUCCESS);
	/* 初始化event */
	status = pj_event_create(pool, NULL, PJ_FALSE, PJ_FALSE, &WorkQ.event);
	pj_assert(status == PJ_SUCCESS);
	
	WorkQ.cnt = 0U;

	/* 创建生产者线程 */	
	status = pj_thread_create(pool, NULL, (pj_thread_proc *)producer, &WorkQ, PJ_THREAD_DEFAULT_STACK_SIZE, 0, &pth);
	pj_assert(status == PJ_SUCCESS);

	/* 创建消费者线程 */
	status = pj_thread_create(pool, NULL, (pj_thread_proc *)consumer, &WorkQ, PJ_THREAD_DEFAULT_STACK_SIZE, 0, &cth);
	pj_assert(status == PJ_SUCCESS);

	pj_thread_join(pth);
	pj_thread_join(cth);

end:
	pj_mutex_destroy(WorkQ.mutex);
	pj_event_destroy(WorkQ.event);
	pj_pool_release(pool);		
	pj_caching_pool_destroy(&caching_pool);
	pj_shutdown();
	exit(EXIT_SUCCESS);
}
