/*exc.c实验一源码*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
int main(void)
{
	pid_t child1,child2,child;
/*创建两个子进程*/
	child1 = fork();
	child2 = fork();
/*子进程1的出错处理*/
	if( child1 == -1 ){
		perror("child1 fork");
		exit(1);
	}
/*在子进程1中调用execlp函数*/
	else if( child1 == 0 ){
		printf("In child1: execute 'ls -l'\n");
		if(execlp("ls","ls","-l",NULL)<0)
			perror("child1 execlp");
	}
/*子进程2的出错处理*/
	if( child2 == -1 ){
		perror("child2 fork");
		exit(1);
	}
/*在子进程2中使其暂停5秒*/
	else if( child2 == 0 ){
		printf("In child2: sleep for 5 seconds and then exit\n");
		sleep(5);
		exit(0);
	}
/*在父进程中等待子进程2的退出*/
	else{
		printf("In father process:\n");
		do{
			child = waitpid( child2, NULL, WNOHANG );
			if( child ==0 ){
				printf("The child2 process has not exited!\n");
				sleep(1);
			}
		}while( child == 0 );
		if( child == child2 )
			printf("Get child2\n");
		else
			printf("Error occured!\n");
	}
}	
