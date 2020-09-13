#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// 1.常规交换，定义第三个临时变量，type可为int或float等
// #define SWAP(a, b, type)          \
//             { type c = 0;         \
//               c = a, a = b, b = c;\
//             }
// 使用__typeof__()获取a的类型，来确定c的类型，减少一个宏参数
#define SWAP(a, b)          \
            { __typeof__(a) c;         \
              c = a, a = b, b = c;\
            }            

// 2.异或交换 异或方法只能用于整形数据，而浮点表示无能为力，不过可以考虑把浮点数据指针强制转为整形来处理，和后面的内存交换思想是一致的。
// 注意：为使下面SWAP3宏不报错，这里只能用,连接，并且不能加{}
#define SWAP0(a, b)    \
              a = a^b, \
              b = a^b, \
              a = a^b
            

// 3.运算交换 (编译产生警告：unsequenced modification and access to 'a')
// 运算交换可能存在运算过程中数据溢出问题 (实测a、b取INT64_MAX、INT_MAX-1有正常交换，并未溢出, 猜测是编译器优化)
// #define SWAP1(a, b)  { long long t = a + b; a = b; b = t - a; printf("t = %lld\n", t); }
#define SWAP1(a, b)     b = (a+b) - (a=b)

// 4.C语言所有数据在内存中都是不会由于数据类型而发生变化的。基于这个思想我们把所有的数据转化为byte来进行交换处理，这样就可以适应各种数据类型了。
#define SWAP2(a, b) \
        {   unsigned char tempBuf[10];           \
            memcpy(tempBuf, &a, sizeof(a)); \
            memcpy(&a, &b, sizeof(b));      \
            memcpy(&b, tempBuf, sizeof(b)); \
        }

// 5.强制转化为同样字节数的整型，再通过SWAP0的异或进行交换
// 注意：实测如果字节数不同，将会改变待处理的数值
#define SWAP3(a, b)   \
        {   (sizeof(a) == sizeof(uint8_t))  ? SWAP0(*(uint8_t*)&a,  *(uint8_t*) &b) :\
            (sizeof(a) == sizeof(uint16_t)) ? SWAP0(*(uint16_t*)&a, *(uint16_t*)&b) :\
            (sizeof(a) == sizeof(uint32_t)) ? SWAP0(*(uint32_t*)&a, *(uint32_t*)&b) :\
            (sizeof(a) == sizeof(uint64_t)) ? SWAP0(*(uint64_t*)&a, *(uint64_t*)&b) : 0;\
        }

// ✅推荐使用SWAP, 其次是SWAP2、SWAP3
// 因为实测SWAP(float, long long)得到结果还符合其数据类型，虽然丢失精度，而后两者测试结果直接全跪

int main(int argc, char *argv[]) {
    long long a = INT64_MAX;
    long long b = INT64_MAX - 1;

    float af = 1.1;
    float bf = 8.8;

    printf("a = %lld, b = %lld\n", a, b);
    SWAP(a, b);
    // SWAP0(a, b);
    // SWAP1(a, b);
    SWAP2(a, b);
    SWAP3(a, b);
    printf("a = %lld, b = %lld\n", a, b);

    printf("af = %f, bf = %f\n", af, bf);
    SWAP(af, bf);
    // SWAP0(af, bf); // invalid operands to binary expression ('float' and 'float')
    // SWAP1(af, bf);
    SWAP2(af, bf);
    SWAP3(af, bf);
    printf("af = %f, bf = %f\n", af, bf);
}

/* 运行结果
a = 9223372036854775807, b = 9223372036854775806
a = 9223372036854775806, b = 9223372036854775807
af = 1.100000, bf = 8.800000
af = 8.800000, bf = 1.100000
*/