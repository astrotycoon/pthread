#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>

typedef struct _marg_t
{
	int a;
	int b;
} marg_t;

typedef struct _mret_t
{
	int x;
	int y;
} mret_t;

void *mythread_fun(void *arg);

int main(int argc, char *argv[])
{
	int ret = 0;
	pthread_t pthid;
	marg_t arg = {100, 100};
	mret_t *mret = NULL;

	ret = pthread_create(&pthid, NULL, mythread_fun, (void *)&arg);
	assert(ret == 0);

	ret = pthread_join(pthid, (void **)&mret);
	assert(ret == 0);
	printf("mret->x = %d, mret->y = %d\n", 
						mret->x, mret->y);
	free(mret);

	exit(EXIT_SUCCESS);
}

void *mythread_fun(void *arg)
{
	marg_t *marg = (marg_t *)arg;
	printf("marg->a = %d, marg->b = %d\n", 
			marg->a, marg->b);
#if 1
	mret_t *mret = (mret_t *)malloc(sizeof(mret_t));
	assert(mret != NULL);
	mret->x = 1;
	mret->y = 1000000;
#endif
	//mret_t mret = {1, 1000};

	return (void *)mret;
}

