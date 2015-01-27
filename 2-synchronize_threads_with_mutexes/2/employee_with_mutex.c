#include <stdio.h>
#include <stdlib.h>
#include "pjlib.h"

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

static pj_mutex_t *mutex;
void *do_loop(void *data)
{
	int num = *(int *)data;

	while (1) {
		pj_mutex_lock(mutex);
		copy_employee(&employees[num - 1], &employee_of_the_day);
		pj_mutex_unlock(mutex);
	}
}

int main(int argc, const char *argv[])
{
	char 			errmsg[PJ_ERR_MSG_SIZE];	
	pj_status_t		status;
	pj_caching_pool	caching_pool;
	pj_pool_t		*pool;
	pj_thread_t		*th1;
	pj_thread_t		*th2;
	int				num1 = 1;
	int 			num2 = 2;
	int 			i = 0;

begin:
	/* 初始化pjlib库 */
	status = pj_init();
	if (status != PJ_SUCCESS) {
		pj_strerror(status, errmsg, sizeof(errmsg));
		exit(EXIT_FAILURE);
	}
	/* 创建内存池工厂 */	
	pj_caching_pool_init(&caching_pool, NULL, 1024 * 1024);
	/* 创建内存池	*/
	pool = pj_pool_create(&caching_pool.factory, NULL, 4096, 4096, NULL);
	if (NULL == pool) {
		pj_strerror(status, errmsg, sizeof(errmsg));
		pj_caching_pool_destroy(&caching_pool);
		pj_shutdown();
		exit(EXIT_FAILURE);
	}
	/* 初始化mutex */
	status = pj_mutex_create_simple(pool, NULL, &mutex);
	if (status != PJ_SUCCESS) {
		pj_strerror(status, errmsg, sizeof(errmsg));
		pj_caching_pool_destroy(&caching_pool);
		pj_shutdown();
		exit(EXIT_FAILURE);
	}

	/* 创建线程1 */
	status = pj_thread_create(pool, NULL, (pj_thread_proc *)do_loop, &num1, PJ_THREAD_DEFAULT_STACK_SIZE, 0, &th1);
	if (status != PJ_SUCCESS) {
		pj_strerror(status, errmsg, sizeof(errmsg));
		pj_pool_release(pool);
		pj_caching_pool_destroy(&caching_pool);
		pj_shutdown();
		exit(EXIT_FAILURE);
	}
	/* 创建线程2 */
	status = pj_thread_create(pool, NULL, (pj_thread_proc *)do_loop, &num2, PJ_THREAD_DEFAULT_STACK_SIZE, 0, &th1);

	/* 主线程 */
	while (1) {
		pj_mutex_lock(mutex);
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
		pj_mutex_unlock(mutex);
	}

end:
	pj_pool_release(pool);
	pj_caching_pool_destroy(&caching_pool);
	/* 销毁pjlib库 */
	pj_shutdown();

	exit(EXIT_SUCCESS);
}
