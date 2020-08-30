/* 编译器将C语言从源代码到可执行文件的过程
C program: Pre-processing -> Compiling -> Assembling -> Linking -> excutable file
*/

/* 预处理常用命令或用法
    分类	                            命令	            解释	            简单示例
01. 条件包含 Conditional inclusion	    #if， #else， #elif， #endif， #ifdef，#ifndef， #undef	    即平时理解的条件编译	#ifdef identifier
02. 源文件包含 Source file inclusion	#include	        头文件/源文件包含	#include <h-char-sequence> new-line
03. 宏替换 Macro replacement	       #define, #, ##	   宏替换	A##B
04. 行控制 Line control	               #line	           行控制。一般很少用	#line digit-sequence new-line
05. 错误指示 Error directive	       #error	           出错指示。做预处理检查，可以输出错误提示信息。	#error pp-tokensopt new-line
06. 空指令 Null directive              #	                   空指。看起来没什么用令	# new-line
07. 预定义宏名 Predefined macro names  __DATE__,  __FILE__, __LINE__, __STDC__,  __TIME__	预定义宏名	
08. Pragma命令/操作 Pragma directive/operator	_Pragma, #pragma	    pragma指令	    #pragma listing on "..\listing.dir"
*/

/* 预处理特点
a.预处理指令由一系列预处理令牌组成，这些预处理令牌以＃预处理令牌开头，它是源文件中的第一个字符或紧跟着包含至少一个换行符的空白，并在下一个换行符结束。

b.文本行不要以#开头，非指令内容也不要用“语法形式”中的内容作为开头

c.在预处理指令中的预处理标记之间（从引入＃预处理标记之后到结束换行符之前），唯一出现的空白字符是空格和水平制表符（包括替换了注释或可能的空格）
*/

// #define EMPTY
//❌EMPTY # include <file.h> // 25:7: error: expected identifier or '(' 【注意#必须在开头】

/* 01.条件包含
控制条件包含的表达式，一定是一个整型常量的。不能包含类型转换和标识符（如C语言中的关键字、枚举常量等），其只认宏与非宏。
我们可以将以下表达式把defined当做一元操作符：defined identifier或defined (identifier)以上如果identifier是一个有效的宏名，
也就是说上文有用了#define进行定义，并且没用#undef来取消这个定义，那么上述表达式的结果为1，否则为0
*/
#define ID
#if defined(ID)
#endif

#if !defined(ID)
#endif

/* 02.源文件包含 : #include是将已存在文件的内容嵌入到当前文件
a.#include <filename> 和 #include "filename" 中 <> 和 "" 有什么区别？  
用尖括号<>括起来，也称为头文件，表示预处理到系统规定的路径中去获得这个文件（即 C 编译系统所提供的并存放在指定的子目录下的头文件）。
找到文件后，用文件内容替换该语句。
双引号""表示预处理应在当前目录中查找文件名为filename的文件，若没有找到，则按系统指定的路径信息，搜索其他目录。
找到文件后，用文件内容替换该语句。

b.#include是否一定要放在文件头，能放在文件中间吗？
可以放在中间或者其他位置，它实际上会将这段include的内容嵌入到指定位置，当然你要留意include之前是否会用到这个内容了。

c.可以#includeC文件吗，例如#include "plus.c"？
可以，甚至可以#include "plus.txt"
*/

/* 03.宏替换 ： 宏的动作只是一个替换，宏是没有类型的。
a.__VA_ARGS__ 只能用于function-like macro。这个__VA_ARGS__是可变参数的宏，是新的C99规范中新增的。
#ifdef DEBUG
✅  #define DEBUG_PRINTF(format, ...) printf(format, ##__VA_ARGS__) // 这个##，的作用是将token（如format等）连接起来，如果token为空，那就不连接。
#else
    #define DEBUG_PRINTF(format, ...)
#endif

b.function-like macro里面的参数一定要是唯一的。
❌#define SUM(a,a)  ((a)+(a)) 这样的用法是错的,error: duplicate macro parameter name 'a'

c.do{ }while(0)用在宏后面，可以保证其内容在替换后不会被拆散，保持其一致性。
✅#define set_on()	do{set_on_func1(1); set_on_func2(1);}while(0)
*/

#include <stdio.h>
#define RUN 2

#if (RUN == 1)
// MIN宏的普通标准写法
#define MIN(x, y)   ((x) < (y) ? (x) : (y))
int main()
{
    double xx = 1.0;
    double yy = MIN(xx++, 1.5);

    printf("xx=%f, yy=%f\n", xx, yy);
}
/* 运行结果
x=3.000000, yy=2.000000
分析：与期望值x=2.000000 y =1.000000有差异
*/
#elif (RUN == 2)
// MIN宏的GNU改进写法
#define MIN(A, B) ({__typeof__(A) __a = (A); __typeof__(B) __b = (B); __a < __b ? __a : __b;})
int main()
{
    double xx = 1.0;
    double yy = MIN(xx++, 1.5);

    printf("xx=%f, yy=%f\n", xx, yy);
}
/* 运行结果
xx=2.000000, yy=1.000000
分析：与期望结果一致
*/
#elif (RUN == 3)
// 关于#和##的使用 : #是将内容字符串化, ##是连接字符串
#define MESSAGE_FOR(a, b) printf(#a" and "#b":We love you!\n")
#define TOKEN_PASTER(n) printf("token"#n" = %d\n", token##n)

#define _STR(x) #x
#define STR(x) _STR(x)

#define ABC(...) #__VA_ARGS__

int main()
{
    MESSAGE_FOR(Jayn, Bob);

    int token34 = 100;
    TOKEN_PASTER(34);

    // 宏中遇到#或##时就不会再展开宏中嵌套的宏了
    char *pc1 = _STR(__FILE__);
    char *pc2 = STR(__FILE__);
    printf("%s %s %s\n", pc1, pc2, __FILE__);

    printf(ABC(123, 456)"\n");
}
/* 运行结果
Jayn and Bob:We love you!
token34 = 100
__FILE__ "C011_pre-processing.c" C011_pre-processing.c
123, 456
*/
#elif (RUN == 4)
/* 04.行控制
可以简单地理解为，可以改变行号的，甚至文件名都可以改变。它的基本形式如下：
# line digit-sequence "s-char-sequenceopt"
其中 digit-sequence是数值，范围为1~2147483647  
    "s-char-sequenceopt"是字符串，可以省略
*/
int main()
{
    #line 12345 "abcdefg.xxxxx"
    printf("%s line:%d\n", __FILE__, __LINE__);
    printf("%s line:%d\n", __FILE__, __LINE__);
    // #error 提示信息：这里运行错误
}
/* 运行结果
abcdefg.xxxxx line:12345
abcdefg.xxxxx line:12346
分析：可以看出，其可以改变下一行内容所在的行号，以及当前文件的文件名。看起来，这货貌似没啥用。
实际上，我们通过这个指令可以固定文件名和行号，以分析某些特定问题。
*/
#elif (RUN == 5)
/* 07.预定义宏名 （特别注意，这些宏名，不可以被#define和#undef等修饰）
预定义宏名	                    含义
__LINE__	                当前源码的行号，是一个整数
__FILE__	                当前源码的文件名，是一个字符串
__DATE__	                源文件的翻译日期，是一个“Mmm dd yyyy”的字符串文字
__TIME__	                源文件的翻译时间，是一个“hh:mm:ss”的字符串文字
__STDC__	                由编译器具体决定
__STDC_HOSTED__	            如果编译器的目标系统环境中包含完整的标准C库，那么这个宏就定义为1，否则宏的值为0
__STDC_VERSION__	        是一个整数199901L
__cplusplus	                如果是在编译一个C++文件，这是一个整数值199711L
*/
#define INFO_INT(x)    printf("%s : %d\n", #x, x);
#define INFO_LONG(x)   printf("%s : %ld\n", #x, x);
#define INFO_CHAR(x)   printf("%s : %s\n", #x, x);
int main()
{
    INFO_INT(__LINE__);
    INFO_CHAR(__FILE__);
    INFO_CHAR(__DATE__);
    INFO_CHAR(__TIME__);
    INFO_INT(__STDC__);
    INFO_INT(__STDC_HOSTED__);
    INFO_LONG(__STDC_VERSION__);
    // INFO_LONG(__cplusplus);
}
/* 运行结果
__LINE__ : 167
__FILE__ : C011_pre-processing.c
__DATE__ : Aug 31 2020
__TIME__ : 07:04:53
__STDC__ : 1
__STDC_HOSTED__ : 1
__STDC_VERSION__ : 201112
*/
#elif (RUN == 6)
/* 08.Pragma命令/操作
pragma 指令控制编译器的实现指定行为，如禁用编译器警告或更改对齐要求。忽略任何不被识别的 pragma 。

标准 pragma
语言标准定义下列三个 pragma ：
#pragma STDC FENV_ACCESS 实参	(1)	
#pragma STDC FP_CONTRACT 实参	(2)	
#pragma STDC CX_LIMITED_RANGE 实参	(3)	
其中 实参 是 ON 、 OFF 和 DEFAULT 之一。
1) 若设为 ON ，则告知编译器程序将访问或修改浮点环境，
   这意味着禁用可能推翻标志测试和模式更改（例如，全局共用子表达式删除、代码移动，及常量折叠）的优化。默认值为实现定义，通常是 OFF 。
2) 允许缩略浮点表达式，即忽略舍入错误和浮点异常的优化，被观察成表达式以如同书写方式准确求值。
   例如，允许 (x*y) + z的实现使用单条融合乘加CPU指令。默认值为实现定义，通常是 ON 。
3) 告知编译器复数的乘法、除法，及绝对值可以用简化的数学公式 。
   换言之，程序员保证传递给这些函数的值范围是受限的。默认值为 OFF 。
注意：不支持这些 pragma 的编译器可能提供等价的编译时选项，例如 gcc 的 -fcx-limited-range 和 -ffp-contract 。

非标准 pragma
1.#pragma once
这个很多编译器都支持，放在头文件里面，让其只参与一次编译，放在头文件重复包含，效果类似于：
#ifndef _XXX_
#define _XXX_
#endif

2.#pragma message
形式如下
#paragma message("output this message")
简单地说，他可以在预处理时输出一串信息，这个在预处理的时候非常有用，我经常用它来输出log。
具体用法可以像这样：
#ifdef _X86
	#pragma message(“_X86 macro activated!”)
#endif

3.#pragma warning
这个是对警告信息的处理，例如：
#pragma warning(disable:4507)
将4507号经过关闭，即你看不到这个警告。
#pragma warning(once:4385)
只让4385这个警告只显示一次。
#pragma warning(error:164)
把164号经过当error显示出来。

 以上还可以合并起来写成：
#pragma warning( disable : 4507; once : 4385; error : 164 )

4.#pragma pack
这个可以改变结构体内存对齐的方式。例如，以下结构体内存可以按1字节对齐：
#pragma pack(1)
struct abc
{
	int a;
	char b;
	short c;
	int d;
};
#pragma pack() // cancel pack(1)

5.#pragma comment
我们可以用其导入一个lib，例如：
#pragma comment ( lib,"wpcap.lib" )

6.还有一个值得一提的是_Pragma，这个是C99新增加的，实际上跟#param一样，但是其有什么特别作用吗？
我们可以把_Pragma放在宏定义后面，因为它不需要这个#，不存在不能展开宏替换问题，例如：
#define LISTING(x) PRAGMA(listing on #x)
#define PRAGMA(x) _Pragma(#x)
LISTING ( ..\listing.dir )
*/
#define _X86
#ifdef _X86
	#pragma message("_X86 macro activated!")
#endif

#pragma warning(disable:4507; once:4385; error:164)

#define LISTING(x) PRAGMA(listing on #x)
#define PRAGMA(x) _Pragma(#x)

int main()
{
    printf("Hello!\n");
}
#endif