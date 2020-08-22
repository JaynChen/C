#include <stdio.h>
#include <string.h>

// C语言基本类型的大小
// 每个平台都不一样，自行测试验证，以下测试结果是64位MacOS上的gcc: Apple clang version 11.0.3
#define BASE_TYPE_SIZE(t)   printf("%12s : %2ld Byte%s\n", #t, sizeof(t), (sizeof(t)>1) ? "s" : "")
void PrintfBaseTypeSize(void)
{
    BASE_TYPE_SIZE(void);
    BASE_TYPE_SIZE(char);
    BASE_TYPE_SIZE(short);
    BASE_TYPE_SIZE(int);
    BASE_TYPE_SIZE(long);
    BASE_TYPE_SIZE(long long);
    BASE_TYPE_SIZE(float);
    BASE_TYPE_SIZE(double);
    BASE_TYPE_SIZE(long double);
    BASE_TYPE_SIZE(void*);
    BASE_TYPE_SIZE(char*);
    BASE_TYPE_SIZE(int*);

    typedef struct {

    } StructNull;
    BASE_TYPE_SIZE(StructNull);
    BASE_TYPE_SIZE(StructNull*);
}

// 大小端测试
void BigOrLittleEndian(void)
{
    int i = 1;
    (*(char *)&i == 1) ? printf("Little-endian\n") : printf("Big-endian\n");
    ((char)0x44332211 == 0x11) ? printf("Little-endian\n") : printf("Big-endian\n");
}
int main(void)
{
    PrintfBaseTypeSize();
    BigOrLittleEndian();

    return 0;
}
/* 输出结果
        void :  1 Byte
        char :  1 Byte
       short :  2 Bytes
         int :  4 Bytes
        long :  8 Bytes
   long long :  8 Bytes
       float :  4 Bytes
      double :  8 Bytes
 long double : 16 Bytes
       void* :  8 Bytes
       char* :  8 Bytes
        int* :  8 Bytes
  StructNull :  0 Byte
 StructNull* :  8 Bytes
Little-endian
Little-endian
 */
/* 小结：
    1.void类型不是空的，占1个字节
    2.C语言空结构体的大小为0（注意：C++的为1）
    3.不管什么类型，指针都是相同大小的(64位系统下指针大小都是8个字节)
    4.当前环境为小端模式（数据的低位在低地址，数据高位在高地址）
*/