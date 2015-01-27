/*waitpid.c*/
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
	pid_t pc,pr;
	pc=fork();
	if(pc<0)
		printf("Error fork.\n");
/*子进程*/
	else if(pc==0){ 
/*子进程暂停5秒*/
		sleep(5);
/*子进程正常退出*/
		exit(0);
	}
/*父进程*/
	else{ 
/*循环测试子进程是否退出*/
		do{ 
/*调用waitpid，且父进程不阻塞*/
			pr=waitpid(pc,NULL,WNOHANG);
/*若子进程还未退出，则父进程暂停1秒*/
			if(pr==0){ 
				printf("The child process has not exited\n");
				sleep(1);
			}
		}while(pr==0);
/*若发现子进程退出，打印出相应情况*/
		if(pr==pc) 
			printf("Get child %d\n",pr);
		else
			printf("some error occured.\n");
	}
}
