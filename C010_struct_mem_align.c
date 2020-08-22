#include <stdio.h>

#define offset(type, member) (size_t) & (((type *)0)->member)
#define STRUCT_E_ADDR(s, e) printf("%5s size = %2ld %16s addr: %p\n", #s, sizeof(s), #s "." #e, &s.e)
#define STRUCT_E_OFFSET(s, e) printf("%5s size = %2ld %16s offset: %2ld\n", #s, sizeof(s), #s "." #e, offset(__typeof__(s), e))
#define STRUCT_E_ADDR_OFFSET(s, e) printf("%5s size = %2ld %16s addr: %p, offset: %2ld\n", #s, sizeof(s), #s "." #e, &s.e, offset(__typeof__(s), e))
/* __typeof__()说明: 
    a.__typeof__()作用等价于typeof()，在标准C中写扩展是以__开头
    b.如果将typeof用于表达式,则该表达式不会执行,只会得到表达式的类型
    c.typeof的作用在于其能够自动推导表达式类型
    int *p1;
    typeof(p1) p2; // 获取p1的类型定义p2,等价于int *p2;
*/

#define RUN 4

// C语言结构体的内存对齐
#if (RUN == 1)
// 1.基本对齐（最大单元字节不超过模数的对齐）
typedef struct
{
    int e_int;
    char e_char;
} S1;
S1 s1;
typedef struct
{
    int e_int;
    double e_double;
} S11;
S11 s11;
int main(void)
{
    STRUCT_E_ADDR_OFFSET(s1, e_int);
    STRUCT_E_ADDR_OFFSET(s1, e_char);

    STRUCT_E_ADDR_OFFSET(s11, e_int);
    STRUCT_E_ADDR_OFFSET(s11, e_double);
}
/* 运行结果
   s1 size =  8         s1.e_int addr: 0x10e658010, offset:  0
   s1 size =  8        s1.e_char addr: 0x10e658014, offset:  4
  s11 size = 16        s11.e_int addr: 0x10e658018, offset:  0
  s11 size = 16     s11.e_double addr: 0x10e658020, offset:  8
*/
/* 结论
a.一般情况下，结构体所占的内存大小并非元素本身大小之和。
b.不严谨地，结构体内存的大小按最大元素大小对齐。
*/
#elif (RUN == 2)
// 2.结构体的最大元素大小超过模数的情况（可用#pragma pack限定对齐系数来实现）
#pragma pack(1) // 对齐系数设为1个字节
typedef struct
{
    int e_int; // 4
    long double e_ld; // 16
} S12;
// #pragma pack()

typedef struct
{
    long long e_ll; // 8
    long double e_ld; // 16
} S13;

typedef struct
{
    char e_char; // 1
    long double e_ld; // 16
} S14;

int main(void)
{
    S12 s12;
    S13 s13;
    S14 s14;

    STRUCT_E_ADDR_OFFSET(s12, e_int);
    STRUCT_E_ADDR_OFFSET(s12, e_ld);
    STRUCT_E_ADDR_OFFSET(s13, e_ll);
    STRUCT_E_ADDR_OFFSET(s13, e_ld);
    STRUCT_E_ADDR_OFFSET(s14, e_char);
    STRUCT_E_ADDR_OFFSET(s14, e_ld);
}
/* 运行结果（64位macOS下测试正常，没有出现超出模数的对齐异常，用#pragma pack()可模拟限定模数的对齐异常）
  s12 size = 32        s12.e_int addr: 0x7ffeeb4688d0, offset:  0
  s12 size = 32         s12.e_ld addr: 0x7ffeeb4688e0, offset: 16
  s13 size = 32         s13.e_ll addr: 0x7ffeeb4688b0, offset:  0
  s13 size = 32         s13.e_ld addr: 0x7ffeeb4688c0, offset: 16
  s14 size = 32       s14.e_char addr: 0x7ffeeb468890, offset:  0
  s14 size = 32         s14.e_ld addr: 0x7ffeeb4688a0, offset: 16

  第1个结构体前#pragma pack(1)，且不启用对齐系数结束标志#pragma pack()的运行结果
  s12 size = 20        s12.e_int addr: 0x7ffeeb3a48d8, offset:  0
  s12 size = 20         s12.e_ld addr: 0x7ffeeb3a48dc, offset:  4
  s13 size = 24         s13.e_ll addr: 0x7ffeeb3a48c0, offset:  0
  s13 size = 24         s13.e_ld addr: 0x7ffeeb3a48c8, offset:  8
  s14 size = 17       s14.e_char addr: 0x7ffeeb3a48a8, offset:  0
  s14 size = 17         s14.e_ld addr: 0x7ffeeb3a48a9, offset:  1
*/
/* 结论
c.对以上结论b进行修正：结构体内存大小应按最大元素大小对齐，如果最大元素大小超过模数，应按模数大小对齐。
d.如果结构体的最大元素大小超过模数，结构体的起始地址是可以被模数整除的。如果，最大元素大小没有超过模数大小，那它的起始地址是可以被最大元素大小整除。
e.这个模数是可以改变的，可以用预编译命令#pragma pack(n)，n=1,2,4,8,16来改变这一系数，其中的n就是你要指定的“对齐系数”。
*/
/*
网上流传一个表：(可能和实际平台环境对不上，以实测为准)
平台    长度/模数 char short int long float double long long long double
Win-32   长度    1     2    4   4       4      8        8       8
         模数    1     2    4   4       4      8        8       8
Linux-32 长度 1 2 4 4 4 8 8 12
         模数 1 2 4 4 4 4 4 4
Linux-64 长度 1 2 4 8 4 8 8 16
         模数 1 2 4 8 4 8 8 16
*/
#elif (RUN == 3)
// 3.结构体元素位置如何摆放能够减小结构体占用的内存空间？
typedef struct {
    int e_int;
    char e_char1;
    char e_char2;
} S2;

typedef struct {
    char e_char1;
    int e_int;
    char e_char2;
} S3;

typedef struct {
    char e_char1;
    short e_short;
    char e_char2;
    int e_int;
    char e_char3;
} S4;
int main(void)
{
    S2 s2;
    S3 s3;
    S4 s4;

    STRUCT_E_ADDR_OFFSET(s2, e_int);
    STRUCT_E_ADDR_OFFSET(s2, e_char1);
    STRUCT_E_ADDR_OFFSET(s2, e_char2);
    STRUCT_E_ADDR_OFFSET(s3, e_char1);
    STRUCT_E_ADDR_OFFSET(s3, e_int);
    STRUCT_E_ADDR_OFFSET(s3, e_char2);
    STRUCT_E_ADDR_OFFSET(s4, e_char1);
    STRUCT_E_ADDR_OFFSET(s4, e_short);
    STRUCT_E_ADDR_OFFSET(s4, e_char2);
    STRUCT_E_ADDR_OFFSET(s4, e_int);
    STRUCT_E_ADDR_OFFSET(s4, e_char3);
}
/* 运行
   s2 size =  8         s2.e_int addr: 0x7ffee785f8e8, offset:  0
   s2 size =  8       s2.e_char1 addr: 0x7ffee785f8ec, offset:  4
   s2 size =  8       s2.e_char2 addr: 0x7ffee785f8ed, offset:  5
   s3 size = 12       s3.e_char1 addr: 0x7ffee785f8d8, offset:  0
   s3 size = 12         s3.e_int addr: 0x7ffee785f8dc, offset:  4
   s3 size = 12       s3.e_char2 addr: 0x7ffee785f8e0, offset:  8
   s4 size = 16       s4.e_char1 addr: 0x7ffee785f8c8, offset:  0
   s4 size = 16       s4.e_short addr: 0x7ffee785f8ca, offset:  2
   s4 size = 16       s4.e_char2 addr: 0x7ffee785f8cc, offset:  4
   s4 size = 16         s4.e_int addr: 0x7ffee785f8d0, offset:  8
   s4 size = 16       s4.e_char3 addr: 0x7ffee785f8d4, offset: 12
*/
/* 分析
S2、S3中的元素类型相同，只是位置不同，编译输出，S2占8字节，S3占12字节。
S2、S3的对齐方式见：./C010_内存对齐1.png
S4的对齐方式见：./C010_内存对齐2.png
*/
/* 结论
我们在定义结构体的时候，尽量把大小相同或相近的元素放一起，以减少结构体占用的内存空间。
*/
#elif (RUN == 4)
// 4.结构体嵌套另一个结构体怎么计算？
typedef struct {
    int e_int;
    char e_char;
} S1;

typedef struct {
    S1 e_s;
    char e_char;
} SS1;

typedef struct {
    short e_short;
    char e_char;
} S6;

typedef struct {
    S6 e_s;
    char e_char;
} SS2;

int main(void)
{
    SS1 ss1;
    STRUCT_E_ADDR_OFFSET(ss1, e_s);
    STRUCT_E_ADDR_OFFSET(ss1, e_char);

    SS2 ss2;
    STRUCT_E_ADDR_OFFSET(ss2, e_s);
    STRUCT_E_ADDR_OFFSET(ss2, e_char);
}
/* 运行结果
  ss1 size = 12          ss1.e_s addr: 0x7ffee38758e0, offset:  0
  ss1 size = 12       ss1.e_char addr: 0x7ffee38758e8, offset:  8
  ss2 size =  6          ss2.e_s addr: 0x7ffee38758d8, offset:  0
  ss2 size =  6       ss2.e_char addr: 0x7ffee38758dc, offset:  4
*/
/* 结论
结构体内嵌套结构体，结构体内的元素并不会和结构体外的元素合并占一个对齐单元。（将嵌套结构体全部进行展开，然后计算合并单元格）
SS1、SS2的对齐方式图见：./C010_内存对齐3.png
*/
#endif