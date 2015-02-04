#include <pthread.h>  
#include <err.h>
#include <time.h>  
#include <stdio.h>
#include <stdlib.h>
  
pthread_mutex_t mutex_a = PTHREAD_MUTEX_INITIALIZER;  
pthread_mutex_t mutex_b = PTHREAD_MUTEX_INITIALIZER;  
  
void* thread1(void* arg)  
{  
    while(1)  
    {  
        /*sleep(1);*/  
        pthread_mutex_lock(&mutex_a);  
        pthread_mutex_lock(&mutex_b);  
  
        printf("[%lu]thread 1 is running! \n", time(NULL));  
  
        pthread_mutex_unlock(&mutex_b);  
        pthread_mutex_unlock(&mutex_a);  
    }  
    return NULL;  
}  
  
void* thread2(void* arg)  
{  
    while(1)  
    {  
        /*sleep(1);*/  
  
        pthread_mutex_lock(&mutex_a);  
        pthread_mutex_lock(&mutex_b);  
        //pthread_mutex_lock(&mutex_a);  
  
        printf("[%lu]thread 2 is running! \n",time(NULL));  
  
        pthread_mutex_unlock(&mutex_b);  
        pthread_mutex_unlock(&mutex_a);  
  
    }  
    return NULL;  
}  
  
int main()  
{  
    pthread_t tid1, tid2;  
    int status;  
  
    status = pthread_create(&tid1, NULL, thread1, NULL);  
    if(status != 0)  
        errx(status, "thread 1");  
  
    status = pthread_create(&tid2, NULL, thread2, NULL);  
    if(status !=0)  
        errx(status, "thread 2");  
  
    status = pthread_join(tid1, NULL);  
    if(status != 0)  
        errx(status, "join thread1");  
  
    status = pthread_join(tid2, NULL);  
    if(status != 0)  
        errx(status, "join thread2");  
}
