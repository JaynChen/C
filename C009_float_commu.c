#include <stdio.h>
#include <stdlib.h>

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