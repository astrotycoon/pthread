/*execl.c*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
	if(fork()==0){
/*����execl������ע������Ҫ����ps�������ڵ�����·��*/
		if(execl("/bin/ps","ps","-ef",NULL)<0)
			perror("execl error!");
	}
}
