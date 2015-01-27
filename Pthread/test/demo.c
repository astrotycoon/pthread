#pragma comment(linker,"/SECTION:.text,RW")
#include <stdio.h>
#ifdef _DEBUG
#   define OFFSET 0x0C
#else
#   define OFFSET 0x01
#endif

int *p;

int p2()
{
	int a;
	a = 2;
	return a;
}

int main()
{
	p = (int *) ((char *) p2 + OFFSET);
	printf("p2==%p,p==%p,*p==%d\n", (char *) p2, p, *p);
	*p = 3;
	printf("p2()==%d\n", p2());
	return 0;
}

//p2==0x00401000,p==0x0040100c,*p==2
//p2()==3
//
