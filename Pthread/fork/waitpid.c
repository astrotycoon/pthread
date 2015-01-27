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
/*�ӽ���*/
	else if(pc==0){ 
/*�ӽ�����ͣ5��*/
		sleep(5);
/*�ӽ��������˳�*/
		exit(0);
	}
/*������*/
	else{ 
/*ѭ�������ӽ����Ƿ��˳�*/
		do{ 
/*����waitpid���Ҹ����̲�����*/
			pr=waitpid(pc,NULL,WNOHANG);
/*���ӽ��̻�δ�˳����򸸽�����ͣ1��*/
			if(pr==0){ 
				printf("The child process has not exited\n");
				sleep(1);
			}
		}while(pr==0);
/*�������ӽ����˳�����ӡ����Ӧ���*/
		if(pr==pc) 
			printf("Get child %d\n",pr);
		else
			printf("some error occured.\n");
	}
}
