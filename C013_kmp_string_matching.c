#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
KMP算法说到底和暴力字符匹配功能上是一模一样的，就是查找匹配字符串在主字符串中的位置，如果只是应用完全可以不用理会它是怎么实现的，
调用几个函数->传递几个参数->得到结果,然后记得他比暴力匹配高效一点就行了。
KMP算法的核心 : 就是尽可能利用更多匹配过程中的信息来减少匹配串与主串的匹配次数从而提高匹配效率。
KMP算法的优越之处在于不再需要重头开始比较，其主串的比较指针是不会倒退的，算法时间复杂度KMP<strstr
*/

#define NEXT_LEN    6

char *Parent = "1234567891212123456789";
char *Child = "121212";
int  next[NEXT_LEN] = {0};

/*  KMP匹配算法查询函数
    str1:主串 str2:模式串 next:next数组
*/ 
char* KMP(const char *str1, const char *str2, int *next)
{
    int i = 0;
    int j = 0;
    char *ret = (char*)str1;

    // 主串结束、模式串成功
    while (i < strlen(str1) && j < (int)strlen(str2)) {
        if ((j == -1) || (str1[i] == str2[j])) {
            // 下一个字符移动
            i++;
            j++;
        } else {
            // 如果匹配不成功通过j(模式串比较失败地址)找到next中下一次与主串比较的模式串地址
            j = next[j];
        }
    }

    // 表示的是模式串全部匹配
    if (j == strlen(str2)) {
        return (ret + i - j);
    } else {
        return NULL;
    }
}

/* KMP匹配算法next数组生成
str:模式串  next:next数组
*/
void getNext(const char *str, int *next)
{
    next[0] = -1;
    int i = 0;
    int j = -1;

    while (i < (strlen(str) - 1)) {
        // 通过模式串自身对比获得next数组值
        if ((j == -1) || (str[i] == str[j])) {
            ++i;
            ++j;
            next[i] = j;
        } else {
            j = next[j];
        }
    }
}

/* 暴力匹配算法
str1:主串 str2:模式串
*/
char *strstr(const char *str1, const char *str2)
{
    char *cp = (char*)str1;
    char *s1, *s2;

    if (!*str2) {
        return ((char*)str1);
    }

    while (*cp) {
        s1 = cp;
        s2 = (char*)str2;
        while (*s1 && *s2 && !(*s1 - *s2)) {
            s1++;
            s2++;
        }

        if (!*s2) {
            return cp;
        }
        cp++;
    }

    return NULL;    
}

int main(int argc, char *argv[])
{
    int result = 0;
    int i = 0;

    // 获得KMP的next数组
    getNext(Child, next);
    for (i = 0; i < NEXT_LEN; i++) {
        printf("Next[%d] = %d\n", i, next[i]);
    }

    // 进行KMP匹配
    result = KMP(Parent, Child, next) - Parent;
    printf("KMP result = %d\n", result);
    // 进行暴力匹配
    result = strstr(Parent, Child) - Parent;
    printf("strstr result = %d\n", result);

    return 0;
}

/* 运行结果
Next[0] = -1
Next[1] = 0
Next[2] = 0
Next[3] = 1
Next[4] = 2
Next[5] = 3
KMP result = 9
strstr result = 9
*/