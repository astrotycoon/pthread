#ifndef __errors_h
#define __errors_h

#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#define err_abort(code,text) do{\
	fprintf(stderr,"%s at \"%s\":%d:%s\n",\
		text,__FILE__,__LINE__,strerror(code));\
	abort();\
}while(0)

#define errno_abort(text) do {\
	fprintf(stderr,"%s at \"%s\":%d:%s\n",\
		text,__FILE__,__LINE__,strerror(errno));\
	abort();\	
}while(0)

#endif


typedef struct alarm_tag
{
	int seconds;
	char message[64];
}alarm_t;

void* alarm_thread(void* arg)
{
	alarm_t* alarm=(alarm_t *)arg;
	int status;
	status=pthread_detach(pthread_self());
	if(status!=0)
	{
		err_abort(status,"Detach thread");
	}
	sleep(alarm->seconds);
	printf("(%d) %s\n",alarm->seconds,alarm->message);
	free(alarm);
	return NULL;
}

int main(int argc,char* argv[])
{
	int status;
	char line[128];
	alarm_t* alarm;
	pthread_t thread;
	while(1)
	{
		printf("Alarm>\n");
		if(fgets(line,sizeof(line),stdin)==NULL) exit(0);
		if(strlen(line)<=1) continue;
		alarm=(alarm_t*)malloc(sizeof(alarm_t));
		if(alarm==NULL)
		{
			errno_abort("Allocate alarm");
		}
		if(sscanf(line,"%d %64[^\n]",&alarm->seconds,alarm->message)<2)
		{
			fprintf(stderr,"Bad command\n");
			free(alarm);
		}
		else
		{
			status=pthread_create(&thread,NULL,alarm_thread,alarm);
			if(status!=0)
			{
				err_abort(status,"Create alarm thread");
			}
		}
	}
	return 0;
}
