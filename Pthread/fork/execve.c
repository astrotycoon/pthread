#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
/*��������б�������NULL��β*/
	char *arg[]={"env",NULL};
	char *envp[]={"PATH=/tmp","USER=sunq",NULL};
	if(fork()==0){
		if(execve("/bin/env",arg,,envp)<0)
			perror("execve error!");
	}
}
