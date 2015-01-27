#include <stdio.h>
#include <err.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

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

pthread_mutex_t	lock = PTHREAD_MUTEX_INITIALIZER;

void *do_loop(void *data)
{
	int num = *(int *)data;

	while (1) {
		pthread_mutex_lock(&lock);
		copy_employee(&employees[num - 1], &employee_of_the_day);
		pthread_mutex_unlock(&lock);
	}
}


int main(int argc, const char *argv[])
{
	pthread_t	th1, th2;
	int			num1 = 1;
	int 		num2 = 2;
	int 		i;

	copy_employee(&employees[0], &employee_of_the_day);

	if (pthread_create(&th1, NULL, do_loop, &num1)) {
		errx(EXIT_FAILURE, "pthread_create() error.\n");
	}
	if (pthread_create(&th2, NULL, do_loop, &num2)) {
		errx(EXIT_FAILURE, "pthread_create() error.\n");
	}

	while (1) {
		pthread_mutex_lock(&lock);
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
		
		printf("lory, employees contents was always consistent\n");
		pthread_mutex_unlock(&lock);
	}
		
	exit(EXIT_SUCCESS);
}
