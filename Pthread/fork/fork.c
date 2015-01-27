/*fork.c*/
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	pid_t result;
/*����fork�������䷵��ֵΪresult*/
	result = fork();
/*ͨ��result��ֵ���ж�fork�����ķ�����������Ƚ��г�����*/
	if(result == -1){
		perror("fork");
		exit;
	}
/*����ֵΪ0�����ӽ���*/
	else if(result == 0){
	printf("The return value is %d\nIn child process!!\nMy PID is %d\n",result,getpid());
}
/*����ֵ����0��������*/
	else 
	{
	printf("The return value is %d\nIn father process!!\nMy PID is %d\n",result,getpid());
	}
}
