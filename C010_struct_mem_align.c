#include <stdio.h>

#define offset(type, member) (size_t) & (((type *)0)->member)
#define STRUCT_E_ADDR(s, e) printf("%5s size = %2ld %16s addr: %p\n", #s, sizeof(s), #s "." #e, &s.e)
#define STRUCT_E_OFFSET(s, e) printf("%5s size = %2ld %16s offset: %2ld\n", #s, sizeof(s), #s "." #e, offset(__typeof__(s), e))
#define STRUCT_E_ADDR_OFFSET(s, e) printf("%5s size = %2ld %16s addr: %p, offset: %2ld\n", #s, sizeof(s), #s "." #e, &s.e, offset(__typeof__(s), e))
#define VAR_ADDR(v)                printf("%5s size = %2ld %10s addr: %p\n", #v, sizeof(v), #v, &v)
// #define BASE_TYPE_SIZE(t)   printf("%18s = %ld\n", "sizeof("#t")", sizeof(t))
#define BASE_TYPE_SIZE(t)          printf("%12s : %2ld Byte%s\n", #t, sizeof(t), (sizeof(t))>1?"s":"")
#define BITFIELD_VAL(s, e)         printf("%12s : %2ld Byte%s, %10s=0x%X\n", #s, sizeof(s), (sizeof(s))>1?"s":"", #s"."#e, s.e)
/* __typeof__()说明: 
    a.__typeof__()作用等价于typeof()，在标准C中写扩展是以__开头
    b.如果将typeof用于表达式,则该表达式不会执行,只会得到表达式的类型
    c.typeof的作用在于其能够自动推导表达式类型
    int *p1;
    typeof(p1) p2; // 获取p1的类型定义p2,等价于int *p2;
*/

#define RUN 7

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
#elif (RUN == 5)
// 5.联合体union的内存对齐
typedef union {
    char e_char;
    int e_int;
} U1;

// #pragma pack(8)
// 此结构体最大元素为double 8个字节，相当于pack(8)
typedef struct {
    int e_int1;
    union {
        char ue_chars[9];
        int ue_int;
    } u;
    double e_double;
    int e_int2;
} SU2;

// 获知平台的大小端
static union {
    char c[4];
    unsigned long l;
} endian_test = { {'l', '?', '?', 'b'} };
#define PRINTF_ENDIANNESS() printf("PRINTF_ENDIANNESS：%c\n", ((char)endian_test.l))

int main()
{
    U1 u1;

    STRUCT_E_ADDR_OFFSET(u1, e_char);
    STRUCT_E_ADDR_OFFSET(u1, e_int);

    SU2 su2;
    STRUCT_E_ADDR_OFFSET(su2, e_int1);
    STRUCT_E_ADDR_OFFSET(su2, u.ue_chars);
    STRUCT_E_ADDR_OFFSET(su2, u.ue_int);
    STRUCT_E_ADDR_OFFSET(su2, e_double);
    STRUCT_E_ADDR_OFFSET(su2, e_int2);

    PRINTF_ENDIANNESS();
}
/* 运行结果
   u1 size =  4        u1.e_char addr: 0x7ffee39b68c0, offset:  0
   u1 size =  4         u1.e_int addr: 0x7ffee39b68c0, offset:  0
  su2 size = 32       su2.e_int1 addr: 0x7ffee39b68c8, offset:  0
  su2 size = 32   su2.u.ue_chars addr: 0x7ffee39b68cc, offset:  4
  su2 size = 32     su2.u.ue_int addr: 0x7ffee39b68cc, offset:  4
  su2 size = 32     su2.e_double addr: 0x7ffee39b68d8, offset: 16
  su2 size = 32       su2.e_int2 addr: 0x7ffee39b68e0, offset: 24
PRINTF_ENDIANNESS：l
*/
/* 结论
联合体实际上跟结构体类似，也没有特别的规则。

注意：使用union时，要留意平台的大小端问题。
百度百科:
大端模式，是指数据的高字节保存在内存的低地址中，而数据的低字节保存在内存的高地址中，这样的存储模式有点儿类似于把数据当作字符串顺序处理：
地址由小向大增加，而数据从高位往低位放；这和我们的阅读习惯一致。 
小端模式，是指数据的高字节保存在内存的高地址中，而数据的低字节保存在内存的低地址中，这种存储模式将地址的高低和数据位权有效地结合起来，
高地址部分权值高，低地址部分权值低。
*/
#elif (RUN == 6)
// 6.位域（Bitfield）
void bitfield_type_size(void)
{
    typedef struct {
        char bf1:1;
        char bf2:1;
        char bf3:1;
        char bf4:3;
    } SB1;

    typedef struct {
        char bf1:1;
        char bf2:1;
        char bf3:1;
        char bf4:7;
    } SB2;

    typedef struct {
        char bf1:1;
        char bf2:1;
        char bf3:1;
        int  bfint:1;
    } SB3;

    typedef struct {
        char bf1:1;
        char bf2:1;
        int  bfint:1;
        char bf3:1;
    } SB4;

    SB1 sb1;
    SB2 sb2;
    SB3 sb3;
    SB4 sb4;
    VAR_ADDR(sb1);
    VAR_ADDR(sb2);
    VAR_ADDR(sb3);
    VAR_ADDR(sb4);

    typedef struct {
        unsigned char bf1:1;
        unsigned char bf2:1;
        unsigned char bf3:1;
        unsigned char bf4:3;
    } SB11;

    typedef union {
        SB11 sb1;
        unsigned char e_char;
    } UB1;
    UB1 ub1;

    STRUCT_E_ADDR_OFFSET(ub1, sb1);
    STRUCT_E_ADDR_OFFSET(ub1, e_char);

    ub1.e_char = 0xf5;
    BITFIELD_VAL(ub1, e_char);
    BITFIELD_VAL(ub1, sb1.bf1);
    BITFIELD_VAL(ub1, sb1.bf2);
    BITFIELD_VAL(ub1, sb1.bf3);
    BITFIELD_VAL(ub1, sb1.bf4);
}

int main()
{
    bitfield_type_size();
}
/* 运行结果
  sb1 size =  1        sb1 addr: 0x7ffeeba428d8
  sb2 size =  2        sb2 addr: 0x7ffeeba428d0
  sb3 size =  4        sb3 addr: 0x7ffeeba428c8
  sb4 size =  4        sb4 addr: 0x7ffeeba428c0
  ub1 size =  1          ub1.sb1 addr: 0x7ffeeba428b8, offset:  0
  ub1 size =  1       ub1.e_char addr: 0x7ffeeba428b8, offset:  0
         ub1 :  1 Byte, ub1.e_char=0xF5
         ub1 :  1 Byte, ub1.sb1.bf1=0x1
         ub1 :  1 Byte, ub1.sb1.bf2=0x0
         ub1 :  1 Byte, ub1.sb1.bf3=0x1
         ub1 :  1 Byte, ub1.sb1.bf4=0x6
*/
/* 规则总结，以下内容仅供参考：
a.结构体的内存大小，并非其内部元素大小之和；
b.结构体变量的起始地址，可以被最大元素基本类型大小或者模数整除；
c.结构体的内存对齐，按照其内部最大元素基本类型或者模数大小对齐；
d.模数在不同平台值不一样，也可通过#pragma pack(n)方式去改变；
e.如果空间地址允许，结构体内部元素会拼凑一起放在同一个对齐空间；
f.结构体内有结构体变量元素，其结构体并非展开后再对齐；
g.union和bitfield变量也遵循结构体内存对齐原则。
*/
#elif (RUN == 7)
// 7.编程为什么要关注结构体内存对齐
/* a.节省内存
在嵌入式软件开发中，特别是内存资源匮乏的小MCU，这个尤为重要。如果优化程序内存，使得MCU可以选更小的型号，对于大批量出货的产品，可以带来更高利润。
*/
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

int main(void)
{
    S2 s2[1024] = {0};
    S3 s3[1024] = {0};

    printf("sizeof s2:%ld, s3:%ld\n", sizeof(s2), sizeof(s3)); // s2的大小为8K，而s3的大小为12K，一放大，就有很明显的区别了。
}
/* 运行结果
sizeof s2:8192, s3:12288
*/
/* b.union的内存对齐需要
对于同一个内存，有时为了满足不同的访问形式，定义一个联合体变量，或者一个结构体和联合体组合的变量。此时就要知道其内存结构是怎么分布的。
   
   c.内存拷贝
有时候，我们在通信数据接收处理时候，往往遇到，数组和结构体的搭配。即，通信时候，通常使用数组参数形式接收，而处理的时候，按照预定义格式去访问处理。
U8 comm_data[10];
typedef struct
{
    U8 id;
    U16 len;
    U8 data[6];
}FRAME;

FRAME* pFram = (FRAME*)comm_data;
此处，必须要理解这个FRAM的内存结构是怎么样的对齐规则。

   e.调试仿真时看压栈数据
在调试某些奇葩问题时，迫不得已，我们会研究函数跳转或者线程切换时的栈数据，遇到结构体内容，肯定要懂得其内存对齐方式才能更好地获得栈内信息。
*/
/* 8.结构体内存对齐实际应用
a. 内存的mapping
假设你要做一个烧录文件，你想往文件头空间128个字节内放一段项目信息（例如程序大小、CRC校验码、其他项目信息等）。
第一反应，你会考虑用一个结构体，定义一段这样的数据，程序运行的时候也定义同样的结构体去读取这个内存。
但是你需要知道结构体大小啊，这个结构体内存对齐的规则还是需要了解的。

b. 单片机寄存器的mapping
在写MCU驱动的时候，访问寄存器的方式有很多种，但是做到清晰明了，适配性好的，往往需要诸多考量。
直接通过整型指针指到特定地址去访问，是没有问题的，但是对于某一类型的寄存器，往往不是一个固定地址，其后面还有一堆子寄存器属性需要配置。
每个地址都通过整型指针访问，那就很多很凌乱。我们可以通过定义一个特定的结构体，用其指针直接mapping到寄存器的base地址。
但是遇到有些地址是空的怎么办？甚至有些寄存器是32位的，有些16位，甚至8位的，各种参差不齐都在里面。那就要考虑结构体内存对齐了，特别是结构体内有不同类型的元素。
*/
#endif