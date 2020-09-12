#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define RUN_NUM	4

#if (RUN_NUM == 1)
// 1.柔性数组概念: 柔性数组简单概括就是能够满足需要变长度的结构体的需求，且能够解决使用数组时内存的冗余和数组的越界问题。
// 柔性数组是能够巧妙地解决数组内存的冗余和数组的越界问题一种方法。
// 柔性数组也叫动态数组
typedef struct {
	int len;
	int array[]; // 结构体中的数组没有占用内存
} SoftArray;

int main(void)
{
	int len = 10;
	printf("SoftArray size:%lu\n", sizeof(SoftArray)); // SoftArray size:4

	return 0;
}

#elif (RUN_NUM == 2)
// 2.定长数组
// 使用定长数组, 作为数据缓冲区, 为了避免造成缓冲区溢出, 数组的大小一般设为足够的空间,
// 而实际使用过程中, 达到最大长度的数据很少, 那么多数情况下, 缓冲区的大部分空间都是浪费掉的。
// 但是优点是使用过程很简单, 数据空间的开辟和释放简单, 无需程序员考虑额外的操作。
typedef struct {
	int len;
	char data[1024]; // 为防止数组溢出，data的长度一般会设置的足够大，导致数组冗余
} SoftArray;

#define CURR_LENGTH 100

int main(void)
{
	SoftArray* pLogger = NULL;

	printf("size:%lu\n", sizeof(SoftArray)); // size:1028
	pLogger = (SoftArray *)malloc(sizeof(SoftArray));
	if (pLogger != NULL) {
		pLogger->len = CURR_LENGTH;
		memcpy(pLogger->data, "Hello World!", CURR_LENGTH);
		printf("%d, %s\n", pLogger->len, pLogger->data); // 100, Hello World!
	}

	free(pLogger);
	pLogger = NULL;

	return 0;
}

#elif (RUN_NUM == 3)
// 3.指针数据包
// 这种写法将数据与结构内存分离，导致的问题就是需要对结构体和数据分别申请和释放内存，
// 这样对于程序员来说无疑是一个灾难，因为这样很容易导致遗忘释放内存造成内存泄露。
typedef struct {
	int len;
	char *data; // 为防止数组溢出，data的长度一般会设置的足够大，导致数组冗余
} __attribute__((__packed__)) SoftArray;

#define CURR_LENGTH 100

int main(void)
{
	SoftArray* pLogger = NULL;

	printf("size:%lu\n", sizeof(SoftArray)); // size:12
	pLogger = (SoftArray *)malloc(sizeof(SoftArray));
	if (pLogger != NULL) {
		pLogger->len = CURR_LENGTH;
		pLogger->data = (char *)malloc(sizeof(char) * CURR_LENGTH);
		if (pLogger->data != NULL) {
			memcpy(pLogger->data, "Hello World!", CURR_LENGTH);
			printf("%d, %s\n", pLogger->len, pLogger->data); // 100, Hello World!
		}
	}

	free(pLogger->data);
	free(pLogger);
	pLogger = NULL;

	return 0;
}

#elif (RUN_NUM == 4)
// 4.柔性数组，注意事项:
// a.柔性数组成员必须定义在结构体里面且为最后元素；
// b.结构体中不能单独只有柔性数组成员；
// c.柔性数组不占内存。
typedef struct {
	int len;
	char data[];
} SoftArray;

#define CURR_LENGTH 100

int main(void)
{
	SoftArray *pLogger = (SoftArray *)malloc(sizeof(SoftArray) + sizeof(char)*CURR_LENGTH);

	printf("size:%lu\n", sizeof(SoftArray)); // size:4
	if (pLogger != NULL) {
		pLogger->len = CURR_LENGTH;
		memcpy(pLogger->data, "Goodnight, Jayn!", CURR_LENGTH);
		printf("%d, %s\n", pLogger->len, pLogger->data); // 100, Goodnight, Jayn!
	}

	free(pLogger);
	pLogger = NULL;

	return 0;
}

/*
总结:
长度为0的数组并不占有内存空间, 而指针方式需要占用内存空间。
由于声明内存连续性的关系，柔性数组成员必须定义在结构体的最后一个，并且不能是唯一的成员。
对于长度为0数组, 在申请内存空间时, 采用一次性分配的原则进行; 对于包含指针的结构体, 申请空间时需分别进行, 释放时也需分别释放。
*/

#endif

/* 答疑解惑
问题 1：0长数组有什么好处❓
正解：结构体最后使用0或1的长度数组的原因，主要是为了方便的管理内存缓冲区，如果你直接使用指针而不使用数组，那么，你在分配内存缓冲区时，
就必须分配结构体一次，然后再分配结构体内的指针一次，（而此时分配的内存已经与结构体的内存不连续了，所以要分别管理即申请和释放）
而如果使用数组，那么只需要一次就可以全部分配出来，反过来，释放时也是一样，使用数组，一次释放，使用指针，得先释放结构体内的指针，
再释放结构体。还不能颠倒次序。其实就是分配一段连续的的内存，减少内存的碎片化。

问题 2：数组和指针有区别吗❓
正解：除开数组作为参数时和指针没有区别之外，其他的还是有区别的。
分析: 在零长数组中，char a[1]里面的a实际是一个常量，等于&a[0]。而char *b是有一个实实在在的指针变量b存在。所以，a=b是不允许的，而b=a是允许的。
*/
