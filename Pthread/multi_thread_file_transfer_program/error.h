#ifndef _ERROR_H_
#   define _ERROR_H_

#   include <errno.h>
#   include <pthread.h>
extern int errno;
 
#   ifndef MAXLINE
#      define MAXLINE 1024
#   endif
extern pthread_mutex_t mutex;
 extern void err_sys(const char *cause, ...);
extern void err_sys_q(const char *cause, ...);
extern void err_msg(const char *cause, ...);
extern void err_msg_q(const char *cause, ...);
extern void t_err_sys(int t_err_code, const char *cause, ...);
 
#endif /* _ERROR_H_ */
