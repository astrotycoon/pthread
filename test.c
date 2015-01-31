#include <stdio.h>
#include <stdlib.h>
#define __USE_GNU
#include <pthread.h>

int main(int argc, const char *argv[])
{
	pthread_mutex_t fastmutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t recmutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
	pthread_mutex_t errchkmutex = PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP;
	pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

	exit(EXIT_SUCCESS);
}
