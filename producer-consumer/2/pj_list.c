#include <stdio.h>
#include <stdlib.h>
#include "pjlib.h"

struct _Data {
	PJ_DECL_LIST_MEMBER(struct _Data);
	int		value;
};

int main(int argc, const char *argv[])
{
	int i; 

	struct _Data *head = (struct _Data *)calloc(1, sizeof(struct _Data));
	head->value = 0;
	/* 1. 初始化 */		
	pj_list_init(head);	

	if (pj_list_empty(head)) {
		printf("the list is empty\n");
	}

	/* 2. 尾部追加 */
	struct _Data *data1 = (struct _Data *)calloc(1, sizeof(struct _Data));
	data1->value = 1;
	pj_list_push_back(head, data1);

	/* 3. */

	printf("the size of list is %d.\n", pj_list_size(head));

	for (i = 0; i < pj_list_size(head); i++) {
				
	}
	
	
	exit(EXIT_SUCCESS);
}
