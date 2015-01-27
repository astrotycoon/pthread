#include <stdio.h>
#include <err.h>
#include <stdlib.h>
#include <pthread.h>

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
	pthread_t	p_thread;	/* the ID of the new thread */
	int			a = 1;
	int 		b = 2;
	int			c = 3;
	
	/* create a new thread that will execute 'do_loop' */
	if (pthread_create(&p_thread, NULL, do_loop, (void *)&a) != 0) {
		errx(EXIT_FAILURE, "pthrea_create() error.\n");	
	}
	if (pthread_create(&p_thread, NULL, do_loop, (void *)&b) != 0) {
		errx(EXIT_FAILURE, "pthrea_create() error.\n");	
	}
	/* run 'do_loop' in the main thread as well */
	do_loop((void *)&c);

	exit(EXIT_SUCCESS);
}
