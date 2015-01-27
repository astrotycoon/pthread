#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include "comfunc.h"
#include "error.h"
#include "record.h"

#define BACKLOG		20

struct thread_param
{
	int clitfd;
	struct sockaddr_in clitaddrin;
};

log_t *logfd = NULL;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static void *deal_requests(void *arg);
static void atlast(void);
static void sig_INT_handler(int signo);

int main(int argc, char **argv)
{
	int listenfd;
	struct sockaddr_in servaddrin, clitaddrin, tempaddrin;
	socklen_t templen, clitlen;
	char line[MAXLINE];
	struct sigaction act;

	atexit(atlast);

	logfd = log_open("logfile.txt", LOG_STDERR | LOG_NOLF);

	act.sa_handler = sig_INT_handler;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	if (-1 == sigaction(SIGINT, &act, NULL))
		log_sys_q(logfd, EMERG, "server install SIGINT handler error");

	signal(SIGPIPE, SIG_IGN);

	if (-1 == (listenfd = socket(AF_INET, SOCK_STREAM, 0)))
		log_sys_q(logfd, EMERG, "server socket error");

	bzero(&servaddrin, sizeof(servaddrin));
	servaddrin.sin_family = AF_INET;
	servaddrin.sin_port = 0;
	servaddrin.sin_addr.s_addr = htonl(INADDR_ANY);
	if (-1 == bind(listenfd, (struct sockaddr *) &servaddrin, sizeof(servaddrin)))
		log_sys_q(logfd, EMERG, "server bind error");

	templen = sizeof(struct sockaddr);
	if (-1 == getsockname(listenfd, (struct sockaddr *) &tempaddrin, &templen))
		log_sys_q(logfd, EMERG, "server getsockname error");
	snprintf(line, sizeof(line), "The server is listen on port: %d\n", ntohs(tempaddrin.sin_port));
	fprintf(stderr, line);
	log_msg(logfd, INFO, line);

	if (-1 == listen(listenfd, BACKLOG))
		log_sys_q(logfd, EMERG, "server listen error");

	for (;;)
	{
		pthread_t tid;
		struct thread_param *t_param = NULL;

		if (NULL == (t_param = (struct thread_param *) malloc(sizeof(struct thread_param))))
		{
			log_msg(logfd, ERROR, "server malloc for thread param failed");
			continue;
		}

		clitlen = sizeof(clitaddrin);
		if (-1 == (t_param->clitfd = accept(listenfd, (struct sockaddr *) &clitaddrin, &clitlen)))
		{
			log_msg(logfd, ERROR, "server accept error");
			continue;
		}

		snprintf(line, sizeof(line), "Got connection from: %s, port: %d\n", inet_ntoa(clitaddrin.sin_addr), ntohs(clitaddrin.sin_port));
		pthread_mutex_lock(&mutex);
		fprintf(stderr, "\n%s", line);
		pthread_mutex_unlock(&mutex);
		log_msg(logfd, INFO, line);

		memcpy(&(t_param->clitaddrin), &clitaddrin, sizeof(clitaddrin));
		pthread_create(&tid, NULL, deal_requests, (void *) t_param);
	}
	return 0;
}

static void *deal_requests(void *arg)
{
	struct thread_param *t = arg;
	char filename[MAX_FILENAME];
	char line[MAXLINE];
	struct hostent *hstent = NULL;

	pthread_detach(pthread_self());

	if (-1 == recvfilename(t->clitfd, filename, sizeof(filename)))
		log_msg(logfd, EMERG, "server read filename error");

	hstent = gethostbyaddr(&(t->clitaddrin.sin_addr.s_addr), 4, AF_INET);
	snprintf(line, sizeof(line), "hostname:\"%s(%d)\" wants to download file: \"%s\"\n", hstent->h_name, ntohs(t->clitaddrin.sin_port), filename);
	pthread_mutex_lock(&mutex);
	fprintf(stderr, line);
	pthread_mutex_unlock(&mutex);
	log_msg(logfd, INFO, line);
	if (-1 == sendfile(t->clitfd, filename))
	{
		textattr(F_RED, B_WHITE, ATTR_DEFAULT);
		log_msg(logfd, EMERG, "server send file to \"%s(%d)\" failed", hstent->h_name, ntohs(t->clitaddrin.sin_port));
		resetattr();
	}
	else
	{
		snprintf(line, sizeof(line), "server send file to \"%s(%d)\"successfully", hstent->h_name, ntohs(t->clitaddrin.sin_port));
		textattr(F_GREEN, B_WHITE, ATTR_DEFAULT);
		pthread_mutex_lock(&mutex);
		fprintf(stderr, line);
		pthread_mutex_unlock(&mutex);
		resetattr();
		log_msg(logfd, INFO, line);
	}

	close(t->clitfd);
	free(arg);
	pthread_exit((void *) 0);
}

static void atlast(void)
{
	log_msg(logfd, INFO, "server is down.");
	log_close(logfd);
	pthread_mutex_destroy(&mutex);
}

static void sig_INT_handler(int signo)
{
	char line[4];

	fputc('\n', stderr);
	log_msg(logfd, EMERG, "server is interrupted by signal SIGINT");
	fprintf(stderr, "server wants to quit[file may still transporting...](y/n)? ");
	getline(line, sizeof(line));
	if ('y' == line[0] || 'Y' == line[0])
		exit(EXIT_FAILURE);
}
