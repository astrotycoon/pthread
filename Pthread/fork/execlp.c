/*execlp.c*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
	if(fork()==0){
/*����execlp�����������൱�ڵ����ˡ�ps -ef������*/
		if(execlp("ps","ps","-ef",NULL)<0)
			perror("execlp error!");
	}
}
