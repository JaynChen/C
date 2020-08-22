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

    uFloatConvert unFloatConvert;
    // c.使用联合体进行数据转化（方便）--以后可封装成函数(注意大小端问题，当前环境下为小端模式)
    // 小端模式：数据低位低地址；大端模式：数据低位高地址。
    // 对于小端模式，数组index越大，地址越高，故[0]存储的最低位数据0x00，[3]存储最高位数据0x40。
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
unFloatConvert.result = 4.250
*/