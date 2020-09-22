#include <stdio.h>
#include <stdlib.h>

#define RUN 2

#if (RUN == 1)
// 1.整型与浮点转化的精度损失
int main(void)
{
    float fVal1  = 0;
    int   sVal1 = 60012502;
    int   sVal2 = 60012501;

    printf("sizeof(int) = %ld\n", sizeof(int));
    printf("sizeof(float) = %ld\n", sizeof(float));

    fVal1 = (float)sVal1;
    printf("fVal1 = %f\n", fVal1);

    fVal1 = (float)(sVal2);
    printf("fVal1 = %f\n", fVal1);

    sVal1 = (int)fVal1;
    printf("sVal1 = %d\n", sVal1);

    sVal2 = (int)fVal1;
    printf("sVal2 = %d\n", sVal2);

    if (sVal1 == sVal2) {
        printf("sVal1 == sVal2\n");
    } else {
        printf("sVal1 != sVal2\n");
    }
}
/* 运行结果
sizeof(int) = 4
sizeof(float) = 4
fVal1 = 60012504.000000
fVal1 = 60012500.000000
sVal1 = 60012500
sVal2 = 60012500
sVal1 == sVal2
*/
/* 分析
现象：2个不相等的int数，经过转换为float，再将float转换回int类型，其结果相等了。
其发生该情况的主要原因是由于4字节float类型变量不能完全覆盖int类型的所有类型数值，所以在进行强制类型转化的过程中会产生精度上的损失。
*/
#elif (RUN == 2)
/* 2.其实对于内存上所保存的数据是没有具体类型的，所谓数据类型仅仅只是以怎样一种方式进行数据的访问罢了，我们通过指针来进行访问就很好的说明
了这个问题，例如int *ptr，表示ptr所指向的地址以int类型进行访问，如果把ptr改为float类型即float *ptr，其便以float类型来访问地址所对应的内存。
*/
#define SET_INT_VAL(addr, val)  *((int*)(&addr)) = val
#define GET_INT_VAL(addr)       *((int*)(&addr))

int main(void)
{
    float fVal1 = 0;
    int   sVal1 = 60012502;

    printf("sizeof(int)   = %ld\n", sizeof(int));
    printf("sizeof(float) = %ld\n", sizeof(float));

    // 接受整型数据到float数据内存
    SET_INT_VAL(fVal1, sVal1);

    // float数据内存获得整型数据
    printf("            sVal  = %d\n", sVal1);              // 原始值
    printf("(int)(float)sVal1 = %d\n", (int)(float)sVal1);  // 直接强转，导致精度丢失
    printf("GET_INT_VAL(fVal) = %d\n", GET_INT_VAL(fVal1)); // 使用宏通过指针类型赋值和取值，保证精度不丢失
}
/* 运行结果
sizeof(int)   = 4
sizeof(float) = 4
            sVal  = 60012502
(int)(float)sVal1 = 60012504
GET_INT_VAL(fVal) = 60012502
*/
/* 分析
通过两个宏进行地址的类型处理转化，对float内存进行直接访问来保存整形数据，同样后面通过地址的直接访问来获得整形数据，这样就可以绕开float类型的处理问题。
同时大家也要注意其本地类型占用字节大小(float)需>=所要转化的类型占用字节大小(int)，否则会造成数据的篡改，这样就会造成更多麻烦。
其实这里仅仅只用float进行存储参数的设计，原本就存在设计上的缺陷，可以通过共联体，或者无类型内存设计方案进行处理，这样就不会出现类似的问题了。
*/
#elif (RUN == 3)
// 3.设计上使用联合体
typedef union _tag_FloatConvert {
    unsigned char byte[4];
    unsigned int uInt; // 小端模式下，实测unsigned long结果也是正确的
    float result;
} uFloatConvert;

int main(void)
{
    unsigned int iVal = 0x40880000;  // 假设iVal为通信中读取的浮点数据所传输过来的16进制格式

    // a.初学拼数据经常犯的错误
    // 很多初学者做浮点通信字节接收，把接收到的数据组织成整形然后直接强制类型转化为浮点，然而这样并不能
    float fVal = (float)iVal;
    printf("*fVal = %.3f\n", fVal);
    printf("iVal = %d\n", iVal);
    // b.正确拼数据:将数据转化为地址，然后使用float指针去读取可得到正确结果
    float *pfVal = (float*)(&iVal);
    printf("*pfVal = %.3f\n", *pfVal);

    /* 常见的操作系统是小端，通讯协议是大端。
    1.常见CPU的字节序
        大端模式：PowerPC、IBM、Sun
        小端模式：x86、DEC
        ARM既可以工作在大端模式，也可以工作在小端模式。
    2.STM32属于小端模式 
    3.大小端转换：
    */
   // 16位字数据
    #define BIG_TO_LITTLE16(A) (( ((uint16_t)(A) & 0xff00) >> 8)  | \
                                ( ((uint16_t)(A) & 0x00ff) << 8))
    // 32位字数据
    #define BIG_TO_LITTLE32(A) (( ((uint32_t)(A) & 0xff000000) >> 24) | \
                                ( ((uint32_t)(A) & 0x00ff0000) >> 8)  | \
                                ( ((uint32_t)(A) & 0x0000ff00) << 8)  | \
                                ( ((uint32_t)(A) & 0x000000ff) << 24))
    typedef union {
        unsigned int val;
        char c[4];
    } UnVal;
    UnVal unVal;
    unVal.val = 0x12345678;
    printf("当前小端模式:c[0]:%x, c[1]:%x, c[2]:%x, c[3]:%x\n", unVal.c[0], unVal.c[1], unVal.c[2], unVal.c[3]);
    unVal.val = BIG_TO_LITTLE32(unVal.val); // 大小端转换
    printf("转换为大端后:c[0]:%x, c[1]:%x, c[2]:%x, c[3]:%x\n", unVal.c[0], unVal.c[1], unVal.c[2], unVal.c[3]);
    /*  低地址                   高地址
        0x12    0x34    0x56    0x78
    大端 c[0]    c[1]    c[2]    c[3]
    小端 c[3]    c[2]    c[1]    c[0]
    */

    // c.使用联合体进行数据转化（方便）--以后可封装成函数(注意大小端问题，当前环境下为小端模式)
    // 小端模式：数据低位低地址；大端模式：数据低位高地址。
    uFloatConvert unFloatConvert;

    unFloatConvert.byte[0] = 0x00;
    unFloatConvert.byte[1] = 0x00;
    unFloatConvert.byte[2] = 0x88;
    unFloatConvert.byte[3] = 0x40;
    // 使用上面byte转换或者下面uint直接转换皆可
    unFloatConvert.uInt = 0x40880000;
    printf("unFloatConvert.result = %.3f\n", unFloatConvert.result);

    return 0;
}

/* 运行结果
*fVal = 1082654720.000
iVal = 1082654720
*pfVal = 4.250
当前小端模式:c[0]:78, c[1]:56, c[2]:34, c[3]:12
转换为大端后:c[0]:12, c[1]:34, c[2]:56, c[3]:78
unFloatConvert.result = 4.250
*/
#endif