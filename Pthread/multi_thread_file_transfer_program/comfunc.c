#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include "comfunc.h"
#include "error.h"
#include "record.h"

/*
 * r_read -- re-read when interrupted by signal SIGINT
 */
ssize_t r_read(int fd, void *buf, size_t len)
{
	size_t bytestoread;
	ssize_t bytesread;
	ssize_t totalbytes;
	char *ptr = NULL;

	for (ptr = buf, bytestoread = len, totalbytes = 0; bytestoread > 0; ptr += bytesread, bytestoread -= bytesread)
	{
		bytesread = read(fd, ptr, bytestoread);
		if (-1 == bytesread && errno != EINTR)	/* an error occurred */
			return -1;
		if (0 == bytesread)		/* read done */
			break;
		if (-1 == bytesread)
			bytesread = 0;
		totalbytes += bytesread;
	}
	return totalbytes;
}								/* end r_read */

/*
 * r_write -- re-write when interrupted by signal SIGINT
 */
ssize_t r_write(int fd, const void *buf, size_t len)
{
	size_t bytestowrite;
	ssize_t byteswritten;
	size_t totalbytes;
	const char *ptr = NULL;

	for (ptr = buf, bytestowrite = len, totalbytes = 0; bytestowrite > 0; ptr += byteswritten, bytestowrite -= byteswritten)
	{
		byteswritten = write(fd, ptr, bytestowrite);
		if (-1 == byteswritten && errno != EINTR)	/* an error occurred */
			return -1;
		if (-1 == byteswritten)
			byteswritten = 0;
		totalbytes += byteswritten;
	}
	return totalbytes;
}								/* end r_write */

/*
 * recvfilename -- receive the filename sent by client
 */
int recvfilename(int sockfd, char *filename, size_t len)
{
	if (NULL == filename)
		return -1;

	if (len > MAX_FILENAME)
		len = MAX_FILENAME;

	filename[0] = '\0';
	if (-1 == read(sockfd, filename, len))
	{
		log_msg(logfd, ERROR, "server read filename error");
		return -1;
	}
	log_msg(logfd, INFO, "server read filename successfully!\n");
	return 0;
}								/* end recvfilename */

/*
 * sendfile -- send file to client
 */
int sendfile(int sockfd, const char *filename)
{
	int fd;
	long file_size, read_left;
	int readlen, writelen;
	char buffer[BUFSIZE];
	struct stat file_stat;

	if (-1 == (fd = open(filename, O_RDONLY)))
	{
		log_msg(logfd, ERROR, "server open file '%s' error", filename);
		return -1;
	}

	if (-1 == fstat(fd, &file_stat))
	{
		log_msg(logfd, ERROR, "server get file-size error");
		return -1;
	}

	file_size = file_stat.st_size;
	read_left = file_size;
	while (read_left > 0)
	{
		if (-1 == (readlen = r_read(fd, buffer, sizeof(buffer))))
		{
			log_msg(logfd, ERROR, "server r_read error");
			return -1;
		}
		read_left -= readlen;

		writelen = readlen;
		if (-1 == r_write(sockfd, buffer, writelen))
		{
			log_msg(logfd, ERROR, "server r_write error");
			return -1;
		}

		if (0 == readlen && read_left != 0)
		{
			log_msg(logfd, WARN, "The file isn't really fully, may be error!");
			return -1;
		}
	}
	close(sockfd);
	return 0;
}								/* end sendfile */

/*
 * getline -- input one line, without '\n' appended
 */
int getline(char buf[], int maxlen)
{
	int n;

	memset(buf, 0, maxlen);
	fflush(stdin);
	fgets(buf, maxlen, stdin);
	n = strlen(buf);
	buf[n - 1] = '\0';			/* drop '\n' */
	return (n - 1);
}

/*
 * chk_file_stat -- chech file's state
 */
int chk_file_stat(const char *filename)
{
	FILE *fp = NULL;

	if (NULL == (fp = fopen(filename, "r")))
		return FILE_NOT_EXIST;
	else if (EOF == fgetc(fp))
	{
		fclose(fp);
		return FILE_EMPTY;
	}
	rewind(fp);
	fclose(fp);
	return FILE_EXISTS_NOT_EMPTY;
}								/* end chk_file_stat */

/*
 * textattr -- set chars' attritube
 */
void textattr(int f_color, int b_color, int cnt_mode)
{
	pthread_mutex_lock(&mutex);
	printf("\033[%d;%d;%dm", cnt_mode, f_color, b_color);
	fflush(stdout);
	pthread_mutex_unlock(&mutex);
}								/* end textattr */
