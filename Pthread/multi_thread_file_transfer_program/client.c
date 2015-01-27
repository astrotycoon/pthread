#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <signal.h>
#include "comfunc.h"
#include "error.h"
#include "record.h"

#define DOT_PERIOD	40

log_t *logfd = NULL;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static void atlast(void);
static void sig_INT_handler(int signo);

int main(int argc, char **argv)
{
	int sockfd;
	struct sockaddr_in servaddrin;
	char line[MAXLINE];
	char filename[MAX_FILENAME];
	char recvbuf[BUFSIZE];
	int fd;
	int len;
	long total_len;
	int dot_number;
	struct sigaction act;

	if (argc != 3)
		err_msg_q("Usage: %s <SERV IP> <SERV PORT>", argv[0]);

	atexit(atlast);

	logfd = log_open("logfile.txt", LOG_STDERR | LOG_NOLF);

	act.sa_handler = sig_INT_handler;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	if (-1 == sigaction(SIGINT, &act, NULL))
		log_sys_q(logfd, EMERG, "client install SIGINT handler error");

	signal(SIGPIPE, SIG_IGN);

	if (-1 == (sockfd = socket(AF_INET, SOCK_STREAM, 0)))
		log_sys_q(logfd, EMERG, "client socket error");

	log_msg(logfd, INFO, "client is running ...");

	bzero(&servaddrin, sizeof(servaddrin));
	servaddrin.sin_family = AF_INET;
	servaddrin.sin_port = htons(atoi(argv[2]));
	inet_pton(AF_INET, argv[1], &servaddrin.sin_addr);

	if (connect(sockfd, (struct sockaddr *) &servaddrin, sizeof(servaddrin)) != 0)
		log_sys_q(logfd, EMERG, "client connect error");

	for (;;)
	{
		fprintf(stderr, "\ninput file want to download: ");
		getline(filename, sizeof(filename));
		if ('\n' == filename[0])
			continue;

		if (chk_file_stat(filename) != FILE_NOT_EXIST)
		{
			fprintf(stderr, "\"%s\" exists, cover it(yes/no)? ", filename);
			getline(line, sizeof(line));
			if (line[0] == 'n' || line[0] == 'n')
				continue;

			if (0 == strcmp(filename, argv[0]))
			{
				snprintf(line, sizeof(line), "cannot override \"%s\", it's running!\n", argv[0]);
				log_msg(logfd, ERROR, line);
				textattr(F_RED, B_WHITE, ATTR_DEFAULT);
				fprintf(stderr, line);
				resetattr();
				continue;
			}
		}
		break;
	}

	if (-1 == (fd = open(filename, O_CREAT | O_WRONLY)))
		log_sys_q(logfd, EMERG, "client open file error");

	if (-1 == r_write(sockfd, filename, strlen(filename) + 1))
	{
		close(fd);
		log_msg(logfd, INFO, "delete the error file\n");
		if (-1 == unlink(filename))
			log_sys_q(logfd, EMERG, "delte the error file failed\n");
		log_sys_q(logfd, EMERG, "client send filename error");
	}

	total_len = -1;
	strcpy(line, "File is transferring:\n");
	fprintf(stderr, line);
	log_msg(logfd, INFO, line);
	while ((len = r_read(sockfd, recvbuf, sizeof(recvbuf))) > 0)
	{
		if (0 == ++dot_number % 2)
		{
			fputc('.', stderr);
			if (dot_number > 2 * DOT_PERIOD)
			{
				fprintf(stderr, "\r                                           \r");
				dot_number = 0;
			}
		}

		total_len += len;
		if (-1 == r_write(fd, recvbuf, len))
		{
			strcpy(line, "client has some error when recving the file.\n");
			fprintf(stderr, line);
			log_msg(logfd, ERROR, line);
			break;
		}
	}

	if (total_len >= 0)
	{
		textattr(F_GREEN, B_WHITE, ATTR_DEFAULT);
		fprintf(stderr, "\nClient has received successfully!\n");
		log_msg(logfd, INFO, "Client has received successfully!\n");
	}
	else
	{
		textattr(F_RED, B_WHITE, ATTR_DEFAULT);
		log_msg(logfd, EMERG, "file \"%s\" is not exist on server!\n", filename);
		close(fd);
		if (-1 == unlink(filename))
			log_sys_q(logfd, EMERG, "delte the error file failed\n");
	}
	resetattr();
	snprintf(line, sizeof(line), "Received %ld bytes\n", ++total_len);
	fprintf(stderr, line);
	log_msg(logfd, INFO, line);
	close(fd);
	close(sockfd);
	return 0;
}

static void atlast(void)
{
	log_msg(logfd, INFO, "client is down.");
	log_close(logfd);
	pthread_mutex_destroy(&mutex);
}

static void sig_INT_handler(int signo)
{
	char line[4];

	fputc('\n', stderr);
	log_msg(logfd, EMERG, "client is interrupted by signal SIGINT");
	fprintf(stderr, "client really wants to quit(yes/no)? ");
	getline(line, sizeof(line));
	if ('y' == line[0] || 'Y' == line[0])
		exit(EXIT_FAILURE);
}
