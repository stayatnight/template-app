#ifndef __test__queue
#define __test__queue
struct test_queue //结构体标签 可以用来声明但是必须加上 struct
{
    int a;
    char b;
}; //末尾可以加上结构体变量，这个只是变量不能用来声明
typedef struct test_queue testQueue;//将struc 重定义声明
int queue_q;
#endif;