#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <pthread.h>
#include "error.h"


/*
 * err_sys -- print system error msg
 */ 
void err_sys(const char *cause, ...) 
{
	
	
	
	
	



/*
 * err_sys -- print system error msg, then quit
 */ 
void err_sys_q(const char *cause, ...) 
{
	
	
	
	
	


	
/*
 * err_msg -- print normal err msg
 */ 
void err_msg(const char *cause, ...) 
{
	
	
	
	
	



/*
 * err_msg -- print normal err msg, then quit
 */ 
void err_msg_q(const char *cause, ...) 
{
	
	
	
	
	


	
/*
 * t_err_sys -- thread print system error msg, the quit
 */ 
void t_err_sys(int thread_err_code, const char *cause, ...) 
{
	
	
	
	
	


	
/*
 * err_handle -- error handle function
 * @errnoflg: if none zero will show sys err, otherwise not
 * @thread_err_code: error code in thread
 * @fmt: err string format wants printing
 * @ap: handle arguments
 */ 
static void err_handle(int errnoflg, int thread_err_code, 
{
	
	
	
	
	
		/*If want to show system error msg */ 
		if (1 == errnoflg)
	{
		
			/*Save errno, because 'strerror' may modify it */ 
			errno_save = errno;
		
	
	
	else if (2 == errnoflg)
		
	
	
	
		/*Output the final error msg */ 
		fflush(stdout);			/*In case stdout and stderr are the same */
	
	
	
	


