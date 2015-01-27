#ifndef _COMFUNC_H
#   define _COMFUNC_H

#   define MAX_FILENAME	255
#   define MAXLINE			1024
#   define BUFSIZE			1024

#   include <pthread.h>
extern pthread_mutex_t mutex;

ssize_t r_read(int fd, void *buf, size_t len);
ssize_t r_write(int fd, const void *buf, size_t len);
int recvfilename(int sockfd, char *filename, size_t len);
int sendfile(int sockfd, const char *filename);
int getline(char *buf, int maxlen);

#   define FILE_NOT_EXIST			0
#   define FILE_EMPTY				1
#   define FILE_EXISTS_NOT_EMPTY	2
int chk_file_stat(const char *filename);

/*Front colors*/
#   define F_BLACK			30
#   define F_RED			31
#   define F_GREEN			32
#   define F_YELLOW		33
#   define F_BLUE			34
#   define F_PURPLE 		35
#   define F_GREEN_BLUE	36
#   define F_GRAY			37

/*Background colors*/
#   define B_BLACK			40
#   define B_RED			41
#   define B_GREEN			42
#   define B_YELLOW		43
#   define B_BLUE			44
#   define B_PURPLE 		45
#   define B_GREEN_BLUE	46
#   define B_GRAY			47
#   define B_WHITE			48

/*Attribution of chars showed*/
#   define ATTR_DEFAULT	0
#   define ATTR_HIGHLIGHT	1
#   define ATTR_UNDETLINE	4
#   define ATTR_BLINK		5
#   define ATTR_REVERSE	7
#   define ATTR_UNVISIABLE	8

extern void textattr(int f_color, int b_color, int cnt_mode);
#   define resetattr() textattr(F_BLACK, B_WHITE, ATTR_DEFAULT)
#endif
