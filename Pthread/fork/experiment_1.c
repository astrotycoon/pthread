/*exc.cʵ��һԴ��*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
int main(void)
{
	pid_t child1,child2,child;
/*���������ӽ���*/
	child1 = fork();
	child2 = fork();
/*�ӽ���1�ĳ�����*/
	if( child1 == -1 ){
		perror("child1 fork");
		exit(1);
	}
/*���ӽ���1�е���execlp����*/
	else if( child1 == 0 ){
		printf("In child1: execute 'ls -l'\n");
		if(execlp("ls","ls","-l",NULL)<0)
			perror("child1 execlp");
	}
/*�ӽ���2�ĳ�����*/
	if( child2 == -1 ){
		perror("child2 fork");
		exit(1);
	}
/*���ӽ���2��ʹ����ͣ5��*/
	else if( child2 == 0 ){
		printf("In child2: sleep for 5 seconds and then exit\n");
		sleep(5);
		exit(0);
	}
/*�ڸ������еȴ��ӽ���2���˳�*/
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
