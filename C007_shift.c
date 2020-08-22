#include <stdio.h>
#include <stdlib.h>

#define SET_BIT(x,n)    (x|(1<<(n-1)))
#define CLEAR_BIT(x,n)  (x&~(1<<(n-1)))

#define RUN_NUM 3

#if (RUN_NUM == 1)
/* 1.逻辑移位和算术移位
逻辑移位:不考虑符号位的移位处理，左移低位补0，右移高位补0，仅仅只是一种逻辑上的移动，所以对于无符号类型一般属于逻辑移位。
算术移位:会考虑符号位的处理，一般算数移位仅针对有符号整形，其左移采用逻辑移位高位移出，低位补零，而右移则是高位用符号位填充，低位移除。
*/
int sVal = 80;
unsigned int uVal = 80;

int main(void)
{
    sVal = sVal << 2; // shll	$2, %ecx
    sVal = sVal >> 2; // sarl	$2, %ecx

    uVal = uVal << 2; // shll	$2, %ec
    uVal = uVal >> 2; // shrl	$2, %ec

    return 0;
}
/* 分析汇编
转成对应的汇编代码后，发现19、22行的汇编移位语句一样，无论有无符号均是逻辑左移。
总结：C语言的整形变量，无符号数移动均为逻辑移位，有符号数左移为逻辑移位，右移为算数移位。
*/
#elif (RUN_NUM == 2)
// 2.有符号左移溢出问题 (有符号数左移竟然是逻辑移位，那这问题可就大了，如果一个正数左移就有可能出现负数)
char sVal1 = 64;
char sVal2 = -7;

int main(int argc, char *argv[])
{
    printf("sVal1 = %d\n", sVal1);
    sVal1 = sVal1 << 1;
    printf("sVal1 << 1 = %d\n", sVal1);

    printf("sVal2 = %d\n", sVal2);
    sVal2 = sVal2 << 1;
    printf("sVal2 << 1 = %d\n", sVal2);

    sVal2 = sVal2 << 4;
    printf("sVal2 << 4 = %d\n", sVal2);

    return 0;
}
/* 运行结果
sVal1 = 64
sVal1 << 1 = -128
sVal2 = -7
sVal2 << 1 = -14
sVal2 << 4 = 32
分析：从上面的结果可以看出，左移运算并不是与*2等价的，其存在溢出问题，由于符号位会被低位代替，
所以其最终的符号由所移动的最后一位决定，大家在使用的时候需要注意。
*/
#elif (RUN_NUM == 3)
/* 3.浮点运算的移位问题
我们对浮点数一般都不进行移位操作，并且大部分编译都是禁止该类语法操作，因为浮点存储格式中具体的bit段是有具体含义的，
并且控制在固定的bit上，比如你把阶码移动到了尾码，那么这个移位操作的结果代表什么意义?根本无法理解，
不过有些小伙伴为了能够获得更高的效率，想直接通过移位来代替*2运算，会考虑直接操作阶码来进行处理，可以参考如下代码:
*/
float fVal = 3.123;
double dVal = 3.123;

// 单精度浮点移位
#define FLOAT_SFT(f, n)     ({ \
                            int itemp = *(int*)&f;\
                            itemp += n * 0x00800000u;\
                            *(float*)&itemp;})
// 双精度浮点移位
#define DOUBLE_SFT(d, n)    ({ \
                            long long lltemp = *(long long*)&d;\
                            lltemp += n * 0x0010000000000000LL;\
                            *(double*)&lltemp;})
int main(void)
{
    printf("FLOAT_SFT(fVal, 1) = %f\n", FLOAT_SFT(fVal, 1));
    printf("FLOAT_SFT(fVal, -1) = %f\n", FLOAT_SFT(fVal, -1));

    printf("DOUBLE_SFT(dVal, 1) = %4.6f\n", DOUBLE_SFT(dVal, 1));
    printf("DOUBLE_SFT(dVal, -1) = %4.6f\n", DOUBLE_SFT(dVal, -1));

    return 0;
}                 
/* 运行结果
FLOAT_SFT(fVal, 1) = 6.246000
FLOAT_SFT(fVal, -1) = 1.561500
DOUBLE_SFT(dVal, 1) = 6.246000
DOUBLE_SFT(dVal, -1) = 1.561500
分析：
a.上面的代码应该还是非常有意思的，可以真正体会到浮点数据的存储机构并且区分float类型和double类型的存储区别；
b.同时使用指针对数据进行转化也得到了非常好的应用，特别是大家在以后通信等字节传输过程中需要使用到拆字节传输，也能够在这个实例中受益。
*/                                       
#endif

// 总结：对于一般的应用尽量不要使用有符号进行移位，一般都是转化为无符号移位处理，如果是为了增加*/2的效率，一定要注意移位的范围和溢出问题。