/*execle*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
/*��������б�������NULL��β*/
	char *envp[]={"PATH=/tmp","USER=sunq",NULL};
	if(fork()==0){
/*����execle������ע������ҲҪָ��env������·��*/
		if(execle("/bin/env","env",NULL,envp)<0)
			perror("execle error!");
	}
}
