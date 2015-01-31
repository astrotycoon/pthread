#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "apr/apr_pools.h"
#include "apr/apr_thread_proc.h"
#include "apr/apr_errno.h"

struct employee {
	int 	number;
	int		id;
	char	first_name[32];
	char	last_name[32];
	char	department[32];
	int 	root_number;
};

/* employees在本程序中是只读的，所以不需要考虑同步问题 */
struct employee employees[] = {
	{ 1, 12345678, "astro", "Bluse", "Accounting", 101 },
	{ 2, 87654321, "Shrek", "Charl", "Programmer", 102 },
};

/* 本程序需要对employee_of_the_day这个全局变量修改，所以需要同步 */
struct employee employee_of_the_day;

void copy_employee(struct employee *from, struct employee *to)
{
	memcpy(to, from, sizeof(struct employee));
}

apr_thread_mutex_t *mutex;

static void *do_loop(apr_thread_t *th, void *data)
{
	int num = *(int *)data;

	while (1) {
		apr_thread_mutex_lock(mutex);
		copy_employee(&employees[num - 1], &employee_of_the_day);
		apr_thread_mutex_unlock(mutex);
	}
}

int main(int argc, const char *argv[])
{
	apr_status_t	status;
	apr_pool_t		*pool;
	apr_thread_t	*th1;	
	apr_thread_t	*th2;	
	char 			errmsg[256];
	int				num1 = 1;
	int				num2 = 2;
	int 			i = 0;

begin:
	copy_employee(&employees[0], &employee_of_the_day);
	/* 初始化pool */
	status = apr_pool_initialize();	
	if (status != APR_SUCCESS) {
		apr_strerror(status, errmsg, sizeof(errmsg));	
		puts(errmsg);
		exit(EXIT_FAILURE);
	}
	/* 创建一个pool*/
	status = apr_pool_create(&pool, NULL);
	if (status != APR_SUCCESS) {
		apr_strerror(status, errmsg, sizeof(errmsg));	
		puts(errmsg);
		apr_pool_terminate();
		exit(EXIT_FAILURE);
	}

	status = apr_thread_mutex_create(&mutex, APR_THREAD_MUTEX_DEFAULT, pool);
	if (status != APR_SUCCESS) {
		apr_strerror(status, errmsg, sizeof(errmsg));	
		puts(errmsg);
		apr_pool_destroy(pool);
		apr_pool_terminate();
		exit(EXIT_SUCCESS);
	}

	status = apr_thread_create(&th1, NULL, do_loop, &num1, pool);
	if (status != APR_SUCCESS) {
		apr_strerror(status, errmsg, sizeof(errmsg));	
		puts(errmsg);
		apr_thread_mutex_destroy(mutex);
		apr_pool_destroy(pool);
		apr_pool_terminate();
		exit(EXIT_SUCCESS);
	}

	status = apr_thread_create(&th2, NULL, do_loop, &num2, pool);
	if (status != APR_SUCCESS) {
		apr_strerror(status, errmsg, sizeof(errmsg));	
		puts(errmsg);
		apr_pool_destroy(pool);
		apr_pool_terminate();
		exit(EXIT_SUCCESS);
	}

	while (1) {
		apr_thread_mutex_lock(mutex);
		struct employee *p = &employees[employee_of_the_day.number - 1];

		if (p->id != employee_of_the_day.id) {
			printf("mismatching 'id', %d != %d (loop '%d')\n", 
								employee_of_the_day.id, p->id, i);
			exit(EXIT_FAILURE);
		}

		if (strcmp(p->first_name, employee_of_the_day.first_name)) {
			printf("mismatching 'first_name', %s != %s (loop '%d')\n",
								employee_of_the_day.first_name, p->first_name, i);
			exit(EXIT_FAILURE);
		}

		if (strcmp(p->last_name, employee_of_the_day.last_name)) {
			printf("mismatching 'last_name', %s != %s (loop '%d')\n",
								employee_of_the_day.last_name, p->last_name, i);
			exit(EXIT_FAILURE);
		}

		if (strcmp(p->department, employee_of_the_day.department)) {
			printf("mismatching 'department', %s != %s (loop '%d')\n",
								employee_of_the_day.department, p->department, i);
			exit(EXIT_FAILURE);
		}

		if (p->root_number != employee_of_the_day.root_number) {
			printf("mismatching 'root_number', %d != %d (loop '%d')\n", 
								employee_of_the_day.root_number, p->root_number, i);
			exit(EXIT_FAILURE);
		}
		i++;
		
		printf("lory, employees contents was always consistent\n");
		apr_thread_mutex_unlock(mutex);
	}

end:	
	apr_thread_mutex_destroy(mutex);
	apr_pool_destroy(pool);
	apr_pool_terminate();
	exit(EXIT_SUCCESS);
}
