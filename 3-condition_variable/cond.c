#include <stdio.h>
#include <err.h>
#include <stdint.h>
#include <stdlib.h>
#define __USE_GNU
#include <pthread.h>
#include <sys/queue.h>

#define NUM_HANDLER_THREADS	(3)

struct request {
	int						value;	/* 请求号  */
	TAILQ_ENTRY(request)	entry;
};

struct __queue_request {
	uint32_t							num_request;	/* 请求个数 */	
	pthread_mutex_t						request_mutex;
	pthread_cond_t						got_request;	
	TAILQ_HEAD(queue_request, request)	head_requests;
	void (*add_request)(int, struct __queue_request *);
};

static void add_request(int num, struct __queue_request *pqueue_request)
{
	struct request	*prequest = (struct request *)malloc(sizeof(struct request));
	if (!prequest) {
		errx(EXIT_FAILURE, "error: out of memory.\n");
	}
	prequest->value = num;

	pthread_mutex_lock(&pqueue_request->request_mutex);	
	TAILQ_INSERT_TAIL(&pqueue_request->head_requests, prequest, entry);
	++pqueue_request->num_request;
#if _DEBUG
	printf("add_request: added request with id '%d'\n", pqueue_request->num_request);
	fflush(stdout);
#endif
	pthread_mutex_unlock(&pqueue_request->request_mutex);	
	pthread_cond_signal(&pqueue_request->got_request);
}

static void *handle_requests_loop(void *arg)
{
	struct __queue_request *pqueue_request = (struct __queue_request *)arg;
#if DEBUG
	printf("Starting thread '%lu'\n", pthread_self());
	fflush(stdout);
#endif
	pthread_mutex_lock(&pqueue_request->request_mutex);	

#ifdef DEBUG
    printf("thread '%lu' after pthread_mutex_lock\n", pthread_self());
    fflush(stdout);
#endif /* DEBUG */

	while (1) {
#ifdef DEBUG
        printf("thread '%lu', num_requests =  %d\n", pthread_self(), pqueue_request->num_request);
        fflush(stdout);
#endif /* DEBUG */
		if (pqueue_request->num_request > 0) {
			pthread_mutex_lock(&pqueue_request->request_mutex);	
			struct request *prequest = TAILQ_FIRST(&pqueue_request->head_requests);	
			pthread_mutex_unlock(&pqueue_request->request_mutex);	
			if (prequest) {
				TAILQ_REMOVE(&pqueue_request->head_requests, prequest, entry);
				printf("Thread '%lu' handled request '%d'\n", pthread_self(), prequest->value);
        		fflush(stdout);
				free(prequest);
				--pqueue_request->num_request;
			}
		} else {
#ifdef DEBUG
            printf("thread '%lu' before pthread_cond_wait\n", pthread_self());
            fflush(stdout);
#endif /* DEBUG */
			pthread_cond_wait(&pqueue_request->got_request, &pqueue_request->request_mutex);
#ifdef DEBUG
            printf("thread '%lu' after pthread_cond_wait\n", pthread_self());
            fflush(stdout);
#endif /* DEBUG */
		}
	}
}

int main(int argc, const char *argv[])
{
	int 					i;
	struct 					timespec delay;
	pthread_t				thread[NUM_HANDLER_THREADS];
	struct __queue_request 	request = {
			.num_request = 0U, 
			.request_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP,
			.got_request = PTHREAD_COND_INITIALIZER,
			.head_requests = TAILQ_HEAD_INITIALIZER(request.head_requests),
			.add_request = add_request,
	};

	for (i = 0; i < NUM_HANDLER_THREADS; i++) {
		if (pthread_create(&thread[i], NULL, handle_requests_loop, &request) != 0) {

		}
	}

	sleep(3);
#if 0
	for (i = 0; i < 600; i++) {
		request.add_request(i, &request);

        if (rand() > 3*(RAND_MAX/4)) { /* this is done about 25% of the time */
            delay.tv_sec = 0;
            delay.tv_nsec = 10;
            nanosleep(&delay, NULL);
        }
	}
#else
	uint64_t ii = 0;
	while (1) {
		request.add_request(ii++, &request);

        if (rand() > 3*(RAND_MAX/4)) { /* this is done about 25% of the time */
            delay.tv_sec = 0;
            delay.tv_nsec = 10;
            nanosleep(&delay, NULL);
        }
	}
#endif
	
	sleep(5);	
	printf("Glory,  we are done.\n");
	
	exit(EXIT_SUCCESS);
}

