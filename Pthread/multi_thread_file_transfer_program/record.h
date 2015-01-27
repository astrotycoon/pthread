/*
 * record.h - for log-files
 * Author: dengxiayehu
 * Mail: dengxiayehu@yeah.net
 * Version: 1.0
 */
#ifndef _RECORD_H
#   define _RECORD_H

#   include <stdio.h>
#   include <semaphore.h>

/* max record length */
#   define LOGLINE_MAX 1024

/* record level */
#   define DEBUG	1
#   define INFO	2
#   define WARN	3
#   define ERROR	4
#   define EMERG	5
#   define FATAL	6

/* record type */
#   define LOG_TRUNC	1<<0
#   define LOG_NODATE	1<<1
#   define LOG_NOLF	1<<2
#   define LOG_NOLVL	1<<3
#   define LOG_DEBUG	1<<4
#   define LOG_STDERR	1<<5
#   define LOG_NOTID	1<<6
#   define LOG_DEFAULT	(LOG_STDERR | LOG_TRUNC)

typedef struct log_t_tag
{
	int fd;
	sem_t sem;
	int flags;
} log_t;

extern log_t *logfd;
// extern pthread_mutex_t mutex;

/*
 * log_open -- open the log file
 * @filename: log filename
 * @flags: options for record
 *		LOG_DEFAULT -	show the most info
 *		LOG_TRUNC	-	drop opened log file
 *		LOG_NODATE	-	ignore the date
 *		LOG_NOLF	-	no new line for every record
 *		LOG_NOLVL	-	do not record the msg's level
 *		LOG_DEBUG	-	do not record msg
 *		LOG_STDERR	-	print on stderr
 *		LOG_NOTID	-	just notice
 * return: succ - log_t(>0), fail - NULL
 */
log_t *log_open(const char *filename, int flags);

/*
 * log_close -- close the log file
 */
void log_close(log_t * log);

void log_msg(log_t * log, unsigned int level, const char *fmt, ...);
void log_msg_q(log_t * log, unsigned int level, const char *fmt, ...);
void log_sys(log_t * log, unsigned int level, const char *fmt, ...);
void log_sys_q(log_t * log, unsigned int level, const char *fmt, ...);
void log_t_sys_q(log_t * log, unsigned int level, int t_err_code, const char *fmt, ...);
#endif
/*++++++++++++++++++++++++++++ End of record.h +++++++++++++++++++++++++++*/
