#ifndef __errors_h
#define __errors_h

#include <sys/types.h>
#include <wait.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#define errno_abort(text) do {\
	fprintf(stderr,"%s at \"%s\":%d:%s\n",\
	text,__FILE__,__LINE__,strerror(errno));\
	abort();\
}while(0)

#endif

int main(int argc,char* argv[])
{
	int status;
	char line[128];
	int seconds;
	pid_t pid;
	char messages[64];

	while(1)
	{
		printf("Alarm>\n");
		if(fgets(line,sizeof(line),stdin)==NULL)
		{
			exit(0);
		}
		if(strlen(line)<=1)continue;
		if(sscanf(line,"%d %64[^\n]",
			&seconds,messages)<2)
		{	
			fprintf(stderr,"Bad command\n");
		}
		else
		{
			pid=fork();
			if(pid==(pid_t)-1)
			{
				errno_abort("Fork");
			}
			if(pid==(pid_t)0)
			{	
				sleep(seconds);	
				printf("(%d) %s\n",seconds,messages);				
				_exit(0);
			}
			else
			{
				do
				{
					pid=waitpid((pid_t)-1,NULL,WNOHANG);
					printf("%d\n",pid);
					if(pid==(pid_t)-1)
					{
						errno_abort("wait for child");
					}
				}while(pid!=(pid_t)0);
				printf("%d\n",pid);
			}
			printf("over:%d\n",pid);
		}
		printf("again.\n");
	}
	return 0;
}			
