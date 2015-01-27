#include <stdio.h>
#include <err.h>
#include <stdlib.h>
#include "lib_acl.h"

/* function to be executed by the new thread */
void *do_loop(void *data)
{
	int i, j;
	int me = *(int *)data;

	for (i = 0; i < 10; i++) {
		for (j = 0; j < 500000000; j++)
			;
		printf("'%d' - Got '%d'\n", me, i);
	}

	/* terminate the thread */
	pthread_exit(NULL);
}

int main(int argc, const char *argv[])
{
	acl_pthread_t 	th;
	int 			a = 1;
	int				b = 2;
	int 			c = 3;

	acl_init();
	if (acl_pthread_create(&th, NULL, do_loop, (void *)&a) != 0) {
		errx(EXIT_FAILURE, "acl_pthread_create() error.\n");
	}		
	if (acl_pthread_create(&th, NULL, do_loop, (void *)&b) != 0) {
		errx(EXIT_FAILURE, "acl_pthread_create() error.\n");
	}		

	do_loop((void *)&c);

	acl_end();

	exit(EXIT_SUCCESS);
}
