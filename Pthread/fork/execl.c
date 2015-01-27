/*execl.c*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
	if(fork()==0){
/*调用execl函数，注意这里要给出ps程序所在的完整路径*/
		if(execl("/bin/ps","ps","-ef",NULL)<0)
			perror("execl error!");
	}
}
