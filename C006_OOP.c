#include <stdio.h>
#include <stdlib.h>

#define RUN_NUM 1

#if (RUN_NUM == 1)
/* 1.C面向对象编程：封装
结构体就是数据的集合，数据就是一个对象的属性和方法，把共用的接口给外部使用，把私用的进行隐蔽，这样就形成了良好的封装。
封装就是决定该对象的使用范围。
*/
// 仿类
typedef struct _tag_Class {
    int val;
    void (*pMethod)(struct _tag_Class* pData);
} sClass;

// 方法
void printVal(sClass *pObject)
{
    printf("Object:%d\n", pObject->val);
}

int main(void)
{
    // 定义2个对象
    sClass stObject1;
    sClass stObject2;
    // 初始化对象属性
    stObject1.val = 1;
    stObject2.val = 2;
    stObject1.pMethod = printVal;
    stObject2.pMethod = printVal;
    // 进行对象的使用
    stObject1.pMethod(&stObject1);
    stObject2.pMethod(&stObject2);

    return 0;
}
/* 运行结果
Object:1
Object:2
*/
#elif (RUN_NUM == 2)
/* 2.C面向对象编程：继承
对于继承性，就是父母有的我都有，并且还会有自己独特的地方(数据)，对于代码里面无非就是属性和方法了，
结合结构体地址总是指向其结构体首地址的特点，利用父子结构体前面字段相同进行直接地址传递以后强制类型转化便可以进行访问，
不过这仅仅只是单继承，对于多继承问题暂时不展开。
*/
// 仿父类
typedef struct _tag_Parent {
    int parentVal; // 父类属性
    void (*pMethod)(struct _tag_Parent* pData); // 父类方法
} sParent;

// 仿子类
typedef struct _tag_Child {
    sParent parentObj; // 继承父类数据和方法
    int childVal; // 子类特殊属性
    void (*pMethod)(struct _tag_Child* pData); // 子类特殊方法
} sChild;

// 父类方法
void ParentPrintVal(sParent* pObject)
{
    printf("Parent object:%d\n", pObject->parentVal);
}

// 子类方法
void ChildPrintVal(sChild* pObject)
{
    printf("Child object:%d\n", pObject->childVal);
}

// 对接父类的公共接口
void printVal(sParent* pObject)
{
    pObject->pMethod(pObject);
}

int main(void)
{
    // 创建父类对象、子类对象
    sParent stParentObj;
    sChild  stChildObj;

    // 构造(初始化对象和方法)
    stParentObj.parentVal = 10;
    stParentObj.pMethod = ParentPrintVal;

    // 初始化子类对象和方法
    stChildObj.parentObj.parentVal = 20;
    stChildObj.parentObj.pMethod = ParentPrintVal;
    stChildObj.childVal = 11;
    stChildObj.pMethod = ChildPrintVal;

    // 调用公共的外部接口
    printVal((sParent*)&stParentObj);
    printVal((sParent*)&stChildObj);

    return 0;
}
/* 运行结果
Parent object:10
Parent object:20
*/
#elif (RUN_NUM == 3)
/* 3.C面向对象编程：多态（ “多态”正规一点讲呢，就是方法的重写和重载，其实质就是一件事情的多种处理方式，也是同一种方法不同的运作方式）
多态简单一点说就是一个接口可以产生多种状态处理，C++中的多态来自编译的多态和运行时的多态，
比如重载包括类重载和函数重载属于编译多态、虚函数等等属于运行时多态，如果完全用C来模拟其实现还是相对比较复杂的，
不过我们的目的仅仅只是说引入面向对象的一些思路来指导C编程，这里对于C编程就简单一点实现函数重载，
在C中通过传递参数就能够实现一个函数的多态，不过这个传参可能需要更加灵活一点。
*/
// 任意数据结构
typedef struct _tag_Data {
    void *pData;
    void (*method)(struct _tag_Data* pData);
} sData;

// 具体实现1
void Add(sData *pData)
{
    int * data = (int *)pData->pData;
    printf("Add = %d\n", (data[0] + data[1]));
}

// 具体实现2
void Sub(sData *pData)
{
    int * data = (int *)pData->pData;
    printf("Sub = %d\n", (data[0] - data[1]));
}

// 多态函数
void Cal(sData *pData)
{
    pData->method(pData);
}

int main(void)
{
    int array[2] = {4, 2};
    sData stData1;
    sData stData2;

    stData1.pData = (void *)array;
    stData1.method = Add;

    stData2.pData = (void *)array;
    stData2.method = Sub;

    Cal(&stData1);
    Cal(&stData2);

    return 0;
}
/* 运行结果
Add = 6
Sub = 2
*/
#endif