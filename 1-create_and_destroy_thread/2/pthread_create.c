#include <stdio.h>
#include <stdlib.h>
#include <pjlib.h>

/* function to be executed by the new thread */
void *do_loop(void *data)
{
	int i, j;
	int me = *(int *)data;

	for (i = 0; i < 10; i++) {
		for (j = 0; j < 500000; j++)
			;
		printf("'%d' - Got '%d'\n", me, i);
	}

	/* terminate the thread */
	pthread_exit(NULL);
}

int main(int argc, const char *argv[])
{
	char errmsg[PJ_ERR_MSG_SIZE];
	pj_status_t 	status;
	pj_caching_pool caching_pool;
	pj_pool_t		*pool = NULL;
	pj_thread_t 	*thread;
	int 			a = 1;
	int 			b = 2;
	
	status = pj_init();
	if (status != PJ_SUCCESS) {
		pj_strerror(PJ_ENOMEM, errmsg, sizeof(errmsg));
		exit(EXIT_FAILURE);
	}

	pj_caching_pool_init(&caching_pool, NULL, 1024 * 1024);	
	pool = pj_pool_create(&caching_pool.factory, NULL, 4096, 4096, NULL);
	if (NULL == pool) {
		pj_strerror(PJ_ENOMEM, errmsg, sizeof(errmsg));
		exit(EXIT_FAILURE);
	}
	
	status = pj_thread_create(pool, NULL, (pj_thread_proc *)do_loop, (void *)&a, PJ_THREAD_DEFAULT_STACK_SIZE, 0, &thread);
	if (status != PJ_SUCCESS) {
		pj_strerror(PJ_ENOMEM, errmsg, sizeof(errmsg));
		exit(EXIT_FAILURE);
	}
	
	do_loop((void *)&b);

	pj_pool_release(pool);
	pj_caching_pool_destroy(&caching_pool);
	pj_shutdown();

	exit(EXIT_SUCCESS);
}
