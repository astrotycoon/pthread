#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>

struct _Data {
	int						value;
	CIRCLEQ_ENTRY(_Data)	circleq_entry;
};

int main(int argc, const char *argv[])
{
#if 1
	CIRCLEQ_HEAD(circleq, _Data)	head = CIRCLEQ_HEAD_INITIALIZER(head);
#else
	CIRCLEQ_HEAD(circleq, _Data)	head;
	CIRCLEQ_INIT(&head);
#endif
	exit(EXIT_SUCCESS);
}
