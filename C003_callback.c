#include <stdio.h>

/* 
一.回调函数概念
a.百度百科:
	回调函数就是一个通过函数指针调用的函数。
	如果你把函数的指针（地址）作为参数传递给另一个函数，当这个指针被用来调用其所指向的函数时，我们就说这是回调函数。
b.Stack Overflow某位大神简洁表述：
	A "callback" is any function that is called by another function which takes the first function as a parameter。
c.理解：
	函数 F1 调用函数 F2 的时候，函数 F1 通过参数给 函数 F2 传递了另外一个函数 F3 的指针，在函数 F2 执行的过程中，函数F2 调用了函数 F3，
	这个动作就叫做回调（Callback），而先被当做指针传入、后面又被回调的函数 F3 就是回调函数。
二.为什么要使用回调函数？
a.解耦
	具体框图见./C003_回调函数.png
b.间接修改不可见库函数的功能
	如果库函数对我们不可见，我们修改不了库函数的实现，也就是说不能通过修改库函数让库函数调用普通函数那样实现，
	那我们就只能通过传入不同的回调函数了，这也就是在日常工作中常见的情况。
*/

#define RUN_NUM 2

#if (RUN_NUM == 1)
// 1.简单回调函数：不带参数的同步回调函数
int Callback_1(void)
{
	printf("%s()\n", __func__);
	return 0;
}

int Callback_2(void)
{
	printf("%s()\n", __func__);
	return 0;
}

int Callback_3(void)
{
	printf("%s()\n", __func__);
	return 0;
}

typedef int (*Callback)(void); // 定义回调函数类型，一个指定格式的函数指针
int Handle(Callback callback)
{
	printf(">>>%s()\n", __func__);
	callback();
	printf("%s()>>>\n", __func__);
	return 0;
}

int main(void)
{
	printf(">>>%s()\n", __func__);
	Handle(Callback_1);
	Handle(Callback_2);
	Handle(Callback_3);
	printf("%s()>>>\n", __func__);
	return 0;
}

/* 运行结果：
>>>main()
>>>Handle()
Callback_1()
Handle()>>>
>>>Handle()
Callback_2()
Handle()>>>
>>>Handle()
Callback_3()
Handle()>>>
main()>>>
[Finished in 0.1s]
*/

#elif (RUN_NUM == 2)
// 2.带参数的回调函数
int Callback_1(int x)
{
	printf("%s(), x=%d\n", __func__, x);
	return 0;
}

int Callback_2(int x)
{
	printf("%s(), x=%d\n", __func__, x);
	return 0;
}

int Callback_3(int x)
{
	printf("%s(), x=%d\n", __func__, x);
	return 0;
}

typedef int (*Callback)(int);
// ️带参数的结构体需要另外增加一个参数x来保存回调函数的参数值
// 可以用一个结构体将回调函数及具体的参数值封装组成一个整体，华为hiaudio项目很多这种结构体
int Handle(Callback callback, int x)
{
	printf(">>>%s()\n", __func__);
	callback(x);
	printf("%s()>>>\n", __func__);
	return 0;
}

int main(void)
{
	printf(">>>%s()\n", __func__);
	Handle(Callback_1, 10);
	Handle(Callback_2, 20);
	Handle(Callback_3, 30);
	printf("%s()>>>\n", __func__);
	return 0;
}

/* 运行结果：
>>>main()
>>>Handle()
Callback_1(), x=10
Handle()>>>
>>>Handle()
Callback_2(), x=20
Handle()>>>
>>>Handle()
Callback_3(), x=30
Handle()>>>
main()>>>
[Finished in 0.1s]
*/

#elif (RUN_NUM == 3)
// 3.参考练习
typedef void (*listen)(int);

listen mlisten[3];

void register_observer(listen obs)
{
	for (int i = 0; i < 3; i++) {
		if (mlisten[i] == 0) {
			mlisten[i] = obs;
			return;
		}
	}
}

void listen0(int i)
{
	printf("%s() received i=%d\n", __func__, i);
}

void listen1(int i)
{
	printf("%s() received i=%d\n", __func__, i);
}

void listen2(int i)
{
	printf("%s() received i=%d\n", __func__, i);
}

void notify_all_observer(int val)
{
	for (int i = 0; i < sizeof(mlisten)/sizeof(mlisten[0]); i++) {
		if (mlisten[i] != 0) {
			mlisten[i](val);
		}
	}
}

int main(void)
{
	int i = 0;
	printf("lis1:%p\n", listen0); // lis1:0x100c34da0
	register_observer(listen0);   // lis2:0x100c34dd0
	printf("lis2:%p\n", listen1);
	register_observer(listen1); 
	register_observer(listen2);

	printf("size:mlisten:%lu, mlisten[0]:%lu\n", sizeof(mlisten), sizeof(mlisten[0])); // size:mlisten:24, mlisten[0]:8

	while (1) {
		scanf("%d", &i);
		printf("%d\n", i);
		notify_all_observer(i);
	}
	return 0;
}

#endif
