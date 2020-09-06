#include <stdio.h>

#define RUN 4

#if (RUN == 1)
// 1.使用宏给结构体初始化:如果频繁使用一个结构体的话，使用宏来给结构体进行赋值是很方便的一种做法。
#define NEW_RECT(length, width)     {(length), (width)}

typedef struct _Rect {
    int length;
    int width;
} Rect;

// RT-Thread的底层gpio驱动中应用了宏给结构体初始化
#define __STM32_PIN(index, gpio, gpio_index)    \
    {                                           \
        index, GPIO##gpio, GPIO_PIN_##gpio_index\
    }
/* STM32  GPIO driver */
// #define GPIOA               ((GPIO_TypeDef *) GPIOA_BASE) 
#define __IO
typedef struct {
  __IO u_int32_t CRL;
  __IO u_int32_t CRH;
  __IO u_int32_t IDR;
  __IO u_int32_t ODR;
  __IO u_int32_t BSRR;
  __IO u_int32_t BRR;
  __IO u_int32_t LCKR;
} GPIO_TypeDef;//除了定义结构体，它还把寄存器做了一个映射，映射到地址

struct pin_index {
    int index;
    GPIO_TypeDef *gpio;
    u_int32_t pin;
};

static const struct pin_index pins[] = {
    #if defined(GPIOA)
        __STM32_PIN(0, A, 0);
        __STM32_PIN(1, A, 1);
        __STM32_PIN(2, A, 2);
        __STM32_PIN(3, A, 3);
        __STM32_PIN(4, A, 4);
        __STM32_PIN(5, A, 5);
        __STM32_PIN(6, A, 6);
        __STM32_PIN(7, A, 7);
        __STM32_PIN(8, A, 8);
        __STM32_PIN(9, A, 9);
        __STM32_PIN(10, A, 10);
        __STM32_PIN(11, A, 11);
        __STM32_PIN(12, A, 12);
        __STM32_PIN(13, A, 13);
        __STM32_PIN(14, A, 14);
        __STM32_PIN(15, A, 15);
    #endif
};

int main(void)
{
    Rect rect = NEW_RECT(10, 5);

    printf("rect length = %d, width = %d\n", rect.length, rect.width);
}
/* 运行结果
rect length = 10, width = 5
*/
#elif (RUN == 2)
// 2.结构体内置函数指针:我们常常构造一些结构体来存储数据，然后在一些函数中使用这些结构体。
// 下次不妨把数据与操作数据的函数绑在一起，更清晰明了。
#define NEW_RECT(length, width)     {(calc_area), (length), (width)}

typedef struct _Rect {
    int (*calc_area)(struct _Rect *pThis);
    int length;
    int width;
} Rect;

int calc_area(struct _Rect *pThis)
{
    return (pThis->length * pThis->width);
}

int main(void)
{
    Rect rect = NEW_RECT(10, 5);

    printf("rect length = %d, width = %d\n", rect.length, rect.width);
    printf("rect area = %d\n", rect.calc_area(&rect));
}
/* 运行结果
rect length = 10, width = 5
rect area = 50
*/
#elif (RUN == 3)
/* 3.void* 
虽然void不能直接修饰变量，但是其可以用于修饰指针的指向即无类型指针void*，无类型指针那就有意义了，无类型指针不是一定要指向无类型数据，而是可以指向任意类型的数据。
void *常常用于函数地封装比较多，当然也有用在其它地方，比如在结构体内定义void*类型的私有指针方便扩展结构体。
我们平时在封装自己的函数时，也可以多考虑看看有没有必要使用void*使得函数地通用性更强一些。
*/
/* vim 删除每行前/后n个字符
删除每行前10个字符   :%s/^.{10}//
删除每行后10个字符   :%s/.{10}$//
删除108~110行前2个字符   :108,110s/^..//
删除108~110行后2个字符   :108,110s/..$//
其中，%表示所有行，s表示替换，正则表达式"/^.{10}//"中，^表示行首；"."表示要删除的字符个数，".{10}"表示删除10个字符，可用10个"."表示；
*/
typedef struct _tag_Add {
    int a;
    int b;
    int result;
} sAdd;

void Add(void *param)
{
    sAdd *p = (sAdd *) param;
    p->result = p->a + p->b;
}

typedef struct _tag_Mul {
    float a;
    float b;
    float result;
} sMul;

void Mul(void *param)
{
    sMul *p = (sMul *) param;
    p->result = p->a * p->b;
}

// 公共的调用接口 
void sCal(void *param, void *fuc)
{
    ((void (*)(void*))fuc)(param);
}

int main(void)
{
    sAdd stAdd;
    sMul stMul;

    //数据初始化 
    stAdd.a = 10;
    stAdd.b = 20;

    stMul.a = 5;
    stMul.b = 5;
    //接口直接用 
    sCal(&stAdd,Add);
    sCal(&stMul,Mul);
    //对应的输出 （隐藏了数据类型）
    printf("a + b = %d\n",stAdd.result);
    printf("a * b = %f\n",stMul.result);
 } 
 /* 运行结果
a + b = 30
a * b = 25.000000
 */
#elif (RUN == 4)
/* 4.动态绑定、回调函数
回调函数可以达到动态绑定的作用，在一定程度上可以降低层与层之间的耦合。
而通过C012_R4回调函数.png图我们可以看到下层模块的函数2调用了上层模块的函数3，这个调用过程与一般的调用过程相反，这个过程叫做回调，
这里上层模块的函数3就是回调函数。回调函数的表现形式是函数指针。

C库stdlib.h中带有一个排序函数：qsort函数。这个排序函数的原型为：
void qsort(void *base, size_t nitems, size_t size, int (*compar)(const void *, const void*));
参数：
base-- 指向要排序的数组的第一个元素的指针。
nitems-- 由 base 指向的数组中元素的个数。
size-- 数组中每个元素的大小，以字节为单位。
compar-- 用来比较两个元素的函数，即函数指针（回调函数）。

int compar(const void *p1, const void *p2);
如果compar返回值小于0（< 0），那么p1所指向元素会被排在p2所指向元素的左面；
如果compar返回值等于0（= 0），那么p1所指向元素与p2所指向元素的顺序不确定；
如果compar返回值大于0（> 0），那么p1所指向元素会被排在p2所指向元素的右面。
*/
#include <stdlib.h>

int compar_int(const void *p1, const void *p2)
{
    return (*((int*)p1) - *((int*)p2));
}

void test_qsort(void)
{
    int arr[5] = {8, 5, 10, 1, 100};

    printf("排序前：");
    for (int i = 0; i < 5; i++) {
        printf("%d ", arr[i]);
    }
    qsort((int*)arr, 5, 4, compar_int);
    printf("\n");

    printf("排序后：");
    for (int i = 0; i < 5; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main(void)
{
    test_qsort();
}
/* 运行结果
排序前：8 5 10 1 100 
排序后：1 5 8 10 100 
*/
#endif