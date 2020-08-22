#include <stdio.h>

#define RUN_NUM 3

#if (RUN_NUM == 1)
/*
输入描述:
输入包括两个正整数a,b(1 <= a, b <= 10^9),输入数据包括多组。
输出描述:
输出a+b的结果
*/
int main(void)
{
    long a,b;
    while(scanf("%ld %ld",&a,&b) != EOF)
    {
        printf("%ld\n",a+b);
    }
}
 
#elif (RUN_NUM == 2) 
/*
输入描述:
输入第一行包括一个数据组数t(1 <= t <= 100)
接下来每行包括两个正整数a,b(1 <= a, b <= 10^9)
输出描述:
输出a+b的结果
*/
int main()
{
    int t ,a ,b;
     
    scanf("%d",&t);
     
    for (int i = 0; i < t; i++){
        scanf("%d %d",&a,&b);
        printf("%d\n",a+b);
    }
     
    return 0;
}
#elif (RUN_NUM == 3)
int main(void)
{
    int t, a, b;

    scanf("%d", &a);
    scanf("%d", &b);
    t = a > b ? a : b;
    while ((t%a != 0) || (t%b != 0))
    {
        t++;
    }
    printf("%d\n", t);
}
#endif
