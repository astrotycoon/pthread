#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE	(10)	/* 缓冲区大小为10 */
char 	*buffer;
sem_t	mutex, empty, full;	/* 互斥信号量 		-- 用于互斥访问buffer 
   							 * 计数信号量empty 	-- buffer中空余量 
   							 * 计数信号量full  	-- buffer中使用量 
   							 */	
int 	x, y;	/* 生产者和消费者在buffer中的下标 */

static void output(void)
{
	int i;
	for (i = 0; i < BUFFER_SIZE; i++) {
		printf("%c ", buffer[i]);
	}
	puts("");
}

static void *producer(void *args)
{
	int num = (int)args;
	int i;
	for (i = 0; i < 30; i++) {
		sem_wait(&empty);	/* 如果buffer已满(empty == BUFFER_SIZE)，则等待 */
							/* 如果buffer未满(empty < BUFFER_SIZE), 则可以生产 */
		sem_wait(&mutex);	/* buffer加锁 */
		printf("生产者%d: %s%02d%s", num, "^^^^^", i, "^^^^^");
		buffer[(x++) % BUFFER_SIZE] = 'A';
		output();
		sem_post(&mutex);	/* buffer解锁 */
		sem_post(&full);	/* */
	}
}

static void *consumer(void *args)	/* 消费者 */
{
	int num = (int)args;
	int i;
	for (i = 0; i < 30; i++) {
		sem_wait(&full);	/* 如果buffer已空(full == 0), 则等待 */
							/* 如果buffer不为空(full > 0), 则可以消费 */
		sem_wait(&mutex);	/* buffer加锁 */
		printf("消费者%d: %s%02d%s", num, "*****", i, "*****");
		buffer[(y++) % BUFFER_SIZE] = 'B';	/* 消费时，赋值为'B' */
		output();	/* 输出buffer值 */
		sem_post(&mutex);	/* buffer解锁 */
		sem_post(&empty);	/* 空余量加1 */
	}
}

int main(int argc, const char *argv[])
{
	int 		i;
	pthread_t	ptid[5];
	pthread_t	ctid[5];

begin:
	x = y = 0;
	buffer = (char *)malloc(BUFFER_SIZE * sizeof(char));
	for (i = 0; i < BUFFER_SIZE; i++) {	/* 初始化buffer数组，默认为'N' */
		buffer[i] = 'N';
	}

	sem_init(&mutex, 0, 1);	/* 初始化互斥信号量mutex为1 */	
	sem_init(&empty, 0, BUFFER_SIZE);	/* 初始化计数信号量empty为BUFFER_SIZE */
	sem_init(&full,  0, 0);	/* 初始化计数信号量full为0 */

	/* 消费者 */
	for (i = 0; i < 5; i++) {
		pthread_create(&ctid[i], NULL, consumer, (void *)i);
	}

	/* 生产者 */
	sleep(5);
	for (i = 0; i < 5; i++) {
		pthread_create(&ptid[i], NULL, producer, (void *)i);
	}
	
end:
	for (i = 0; i < 5; i++) {
		pthread_join(ptid[i], NULL);
	}	
	for (i = 0; i < 5; i++) {
		pthread_join(ctid[i], NULL);
	}	

	exit(EXIT_SUCCESS);
}
