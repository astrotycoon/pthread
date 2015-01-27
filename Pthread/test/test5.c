#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>

static int thread_exit = 0;     /* running state of a thread */

void *showtime(void *arg)
{                               /* this is a long-time test */
        int *i = arg;
        time_t p;
        thread_exit = 0;
        while (--(*i) > 0) {
                printf("---%d---\n", *i);
                time(&p);
                printf("%s", ctime(&p));
                sleep(1);
        }
        thread_exit = 1;
        return ((void *)0);
}

int main(int argc, char **argv)
{
        pthread_t tid;
        pthread_attr_t attr;
        void *tret;
        char cmd[16] = { 0 };
        int err, times = 10;    /* default: test costs 10 secs */
        int flag;

        /* Use pthread */
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        err = pthread_create(&tid, NULL, showtime, &times);     /* showtime is a long-time test */
        if (err != 0) {
                perror("Can't create thread.\n");
                exit(1);
        }

        /* pthread_join(tid,&tret); *//* wait thread to terminate, pthread_join will block the main thread, so we give it up */

#if 1
        flag = fcntl(0, F_GETFL, 0);
        flag |= O_NONBLOCK;
//		flag &= ~O_NONBLOCK;
        if (fcntl(0, F_SETFL, flag) < 0) {      /* fgets no-block now */
                perror("Set stdin to non-block fails.");
                exit(1);
        }
#endif

        for (;;) {
                if (thread_exit)
                        break;
                if (fgets(cmd, sizeof(cmd) - 1, stdin) != NULL) {	// 如果这个程序不把stdin设置成非阻塞的话，那么就会一直卡在这里
                        if (strncmp(cmd, "stop", 4) == 0) {
                                pthread_cancel(tid);    /* cancel thread */
                                pthread_attr_destroy(&attr);
                                flag &= ~O_NONBLOCK;
                                if (fcntl(0, F_SETFL, flag) < 0) {      /* block stdin */
                                        perror("Set stdin to block fails.");
                                        exit(1);
                                }
                                break;
                        }
                } else {
                        usleep(10000);
                        continue;       /* not a must */
                }
        }

        return 0;
}
