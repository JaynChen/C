/*
程序构建大概需要经历四个过程：预处理、编译、汇编、链接，这里主要介绍链接这一过程。

1.可用file命令查看文件类型：
file a.o
a.o: Mach-O 64-bit object x86_64
file a.s
a.s: assembler source text, ASCII text
file C004_a+b.c
C004_a+b.c: c program text, UTF-8 Unicode text

2.目标文件的构成

目标文件主要分为文件头、代码段、数据段和其它。

文件头：描述整个文件的文件属性（文件是否可执行、是静态链接还是动态链接、入口地址、目标硬件、目标操作系统等信息），
还包括段表，用来描述文件中各个段的数组，描述文件中各个段在文件中的偏移位置和段属性。

代码段：程序源代码编译后的机器指令。

数据段：数据段分为.data段和.bss段。
.data段内容：已经初始化的全局变量和局部静态变量
.bss段内容：未初始化的全局变量和局部静态变量，.bss段只是为未初始化的全局变量和局部静态变量预留位置，本身没有内容，不占用空间。

除了代码段和数据段，还有.rodata段、.comment、字符串表、符号表和堆栈提示段等等，还可以自定义段
*/

#include <stdio.h>

#define RUN_NUM 3

#if (RUN_NUM == 3)
// 3. .bss段不占用存储空间？
int a[1000] = {1};
int b[1000] = {1};

int main()
{
	printf("Good evening!\n");
	return 0;
}
/* 查看文件大小和各个段大小 
a.int a[1000];
gcc -o C005 C005_link_segment.c
ls -l C005
-rwxr-xr-x  1 jaynchen  staff  12612  8 15 20:56 C005
size C005
__TEXT  __DATA  __OBJC  others  		dec     		hex
4096    8192    0       4294975488      4294987776      100005000

b.int a[1000] = {1};
ls -l C005
-rwxr-xr-x  1 jaynchen  staff  16708  8 15 21:03 C005
size C005
__TEXT  __DATA  __OBJC  others  		dec     		hex
4096    8192    0       4294975488      4294987776      100005000
*/

/* 4.程序为什么要分成数据段和代码段

a.数据和指令被映射到两个虚拟内存区域，数据段对进程来说可读写，代码段是只读，这样可以防止程序的指令被有意无意的改写。
b.有利于提高程序局部性，现代CPU缓存一般被设计成数据缓存和指令缓存分离，分开对CPU缓存命中率有好处。
c.代码段是可以共享的，数据段是私有的，当运行多个程序的副本时，只需要保存一份代码段部分

/* 5.链接器通过什么进行的链接

链接的接口是符号，在链接中，将函数和变量统称为符号，函数名和变量名统称为符号名。
链接过程的本质就是把多个不同的目标文件之间相互“粘”到一起，像玩具积木一样各有凹凸部分，有固定的规则可以拼成一个整体。

可以将符号看作是链接中的粘合剂，整个链接过程基于符号才可以正确完成，符号有很多类型，主要有局部符号和外部符号，局部符号只在编译单元内部可见，
对于链接过程没有作用，在目标文件中引用的全局符号，却没有在本目标文件中被定义的叫做外部符号，以及定义在本目标文件中的可以被其它目标文件引用
的全局符号，在链接过程中发挥重要作用。
*/

#elif (RUN_NUM == 6)
/* 6. 强符号和弱符号
我们经常编程中遇到的multiple definition of 'xxx'，指的是多个目标中有相同名字的全局符号的定义，产生了冲突，这种符号的定义指的是强符号。
有强符号自然就有弱符号，编译器默认函数和初始化了的全局变量为强符号，未初始化的全局变量为弱符号。
__attribute__((weak))可以定义弱符号。
*/

__attribute__ ((weak)) void foo(void);
// 以上在mac下的clang测试验证失败，可能在linux下gcc是ok的
/* Undefined symbols for architecture x86_64:
  "_foo", referenced from:
      _main in C005_link_segment-09408a.o
ld: symbol(s) not found for architecture x86_64
clang: error: linker command failed with exit code 1 (use -v to see invocation)
*/
// 这种强引用弱引用对于库来说十分有用，库中的弱引用可以被用户定义的强引用所覆盖，这样程序就可以使用自定义版本的库函数，
// 可以将引用定义为弱引用，如果去掉了某个功能，也可以正常连接接，想增加相应功能还可以直接增加强引用，方便程序的裁剪和组合。

int main() {
	if (foo) {
		foo();
	}
	return 0;
}

// test2.c
// #include <stdio.h>

// void foo() {
//    printf("foo2\n");
// }

// test3.c
// #include <stdio.h>

// void foo() {
//    printf("foo3\n");
// }

#endif