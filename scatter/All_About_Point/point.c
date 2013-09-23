#include<stdio.h>
void main()
{
	int * p = NULL;
	int num = 5;

	//p指向NULL
	printf("%08x\n",p);
	//printf("%08x\n",*p);  //错误：无法编译
	printf("\n");
	
	//p指向一个整型
	p = &num;
	printf("%08x\n",p);
	printf("%08x\n",*p);
	printf("\n");

	//修改p所指内容的值
	*p = 6;
	printf("%08x\n",p);
	printf("%08x\n",*p);
	printf("\n");

	//p指向一个动态分配的空间
	p = (int*)malloc(sizeof(int));
	*p = 7;
	printf("%08x\n",p);
	printf("%08x\n",*p);
	free(p);
	printf("\n");

	//p指向已经回收过的空间（错误使用），所以free之后要马上令指针指向NULL
	printf("%08x\n",p);
	printf("%08x\n",*p);
	printf("\n");

	//对已经回收过的空间赋值（错误使用）
	*p = 8;
	printf("%08x\n",p);
	printf("%08x\n",*p);
	printf("\n");
	
}
