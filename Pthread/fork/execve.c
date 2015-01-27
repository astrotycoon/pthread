#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
/*命令参数列表，必须以NULL结尾*/
	char *arg[]={"env",NULL};
	char *envp[]={"PATH=/tmp","USER=sunq",NULL};
	if(fork()==0){
		if(execve("/bin/env",arg,,envp)<0)
			perror("execve error!");
	}
}
