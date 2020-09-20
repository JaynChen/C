#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define RUN 3

#if (RUN == 1)
/* 1.【IPC】共享内存 - 建立内存映射文件
   如何建立内存映射文件？首先，需要使用共享内存的进程使用open函数打开指定的文件，然后进程使用mmap将之映射到自己的地址空间中，
   在mmap映射建立完成之后，我们就可以使用close关闭掉文件了，这对于已经建立映射没有影响。
   下面一段代码使用了内存映射文件，由A进程产生一个随机数并共享给B进程，由B进程打印出来。
   通过这种手法进行的共享内存，内存的内容将保存在文件中，这并不需要我们自身进行任何操作，全部交由内核代劳。
*/
void err(const char *str)
{
    perror(str);
    exit(EXIT_FAILURE);
}

int main(int argc, const char *argv[])
{
    // 1.使用open打开指定的内存映射文件
    int fd = open("/tmp/example.shm", O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        err("open");
    }

    // 2.文件可能之前不存在，没有页面，此时直接使用将引发一个bus error，先将其截断为指定大小，然后才可以进行相关读取
    if (ftruncate(fd, sizeof(int)) == -1) {
        err("ftruncate");
    }

    // 3.调用mmap将内存映射文件映射到自身的地址空间中
    int *p = mmap(NULL, sizeof(int), PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if (p == MAP_FAILED) {
        err("mmap");
    }

    // 4.完成映射之后可以关闭fd，不影响已经建立的映射
    close(fd);

    // 5.产生随机数并存入共享内存中，使用sleep为了便于观察
    for (srand(7777); ; sleep(1)) {
        int v = rand();
        *p = v;
        printf("produce: %d\n", v);
    }

    // 6.解除内存映射
    if (munmap(p, sizeof(int)) == -1) {
        err("munmap");
    }

    return EXIT_SUCCESS;
}

/* a文件运行结果
produce: 130708039
produce: 2081724239
produce: 735707949
produce: 1980143064
produce: 710399089
produce: 1815895150
produce: 1859678533
produce: 1140105693
produce: 1907283717
produce: 229032850
...

b文件运行结果
1297830119
1297830119
1297830119
1297830119
1297830119
130708039  // a文件从这里开始运行
2081724239
735707949
1980143064
710399089
1815895150
1859678533
1140105693
1907283717
229032850
...
*/
#elif (RUN == 2)
/* 2.【IPC】共享内存 - 匿名映射
上面的方法中，我们将不得不在文件系统中建立一个文件，若我们想要在具有亲缘关系的进程中共享内存，我们可以采用“匿名映射”的方法取代建立文件，
具体方法有两种：
1.打开/dev/zero文件以取代之前的文件
2.使用MAP_ANON参数并将fd参数指定为-1
以下是使用匿名映射的例子，在该例中，父进程负责在共享内存中产生随机数，而子进程则负责打印父进程产生的随机数。
*/
// 注释部分代码为方法1，macos vscode实测失败，输出结果：map: Operation not supported by device

void err(const char *str)
{
    perror(str);
    exit(EXIT_FAILURE);
}

int main(int argc, const char *argv[])
{
    // int fd = open("/dev/zero", O_RDWR, 0666);
    // if (fd == -1) {
    //     err("open");
    // }

    // int *p = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    int *p = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
    // 或者MAP_SHARED | MAP_ANONYMOUS

    if (p == MAP_FAILED) {
        err("mmap");
    }

    // close(fd);

    pid_t id = fork();
    if (id == 0) {
        for (; ; sleep(1)) {
            printf("%d\n", *p);
        }
    } else {
        for (srand(7777); ; sleep(1)) {
            int v = rand();
            *p = v;
            printf("produce: %d\n", v);
        }
    }

    if (munmap(p, sizeof(int)) == -1) {
        err("munmap");
    }

    return EXIT_SUCCESS;
}
/* 运行结果
produce: 130708039
130708039
130708039
produce: 2081724239
2081724239
produce: 735707949
735707949
produce: 1980143064
1980143064
produce: 710399089
710399089
produce: 1815895150
1815895150
produce: 1859678533
1859678533
produce: 1140105693
1140105693
...
*/
#elif (RUN == 3)
/* 3.【IPC】共享内存 - 共享内存对象
共享内存区对象具有两种实现，由于其与内存映射的用法高度统一且可以使用一些与文件相关的系统调用，与习惯比较一致，所以这里仅介绍POSIX共享内存区。
POSIX共享内存区对象提供了以下两个函数：

int shm_open(const char *name, int oflag, mode_t mode);
int shm_unlink(const char *name);
这两个函数的直观感觉与文件操作的open函数与unlink函数一致，实际上他们的使用方法也近乎一致，区别仅仅在于shm_open函数和shm_unlink函数操作
的对象是共享内存区对象，而非普通的文件。shm_open函数在调用成功时将返回共享内存区对象的文件描述符。

并且，和普通的unlink函数一样，使用shm_unlink函数删除之后，并不影响已经打开该共享内存区对象的进程接着使用它。
新创建的共享内存区对象同新创建的内存映射文件一样，需要先使用ftruncate函数将之改为我们需要的大小。

使用shm_open创建共享内存区对象之后，既可以使用read和write系统调用使用，也可以像内存映射文件一样使用mmap将之映射到内存地址空间。
不过，既然使用共享内存是为了更高的效率，那么我们就应当使用mmap。用法与之前的内存映射文件没有太大的区别。

文件系统抽象：
使用shm_open函数创建的共享内存区对象将被存放在/dev/shm目录下，我们可以通过ls /dev/shm查看当前的共享内存区对象，
并且，我们也可以使用mount命令或者df命令看到其在文件系统中的挂载点。

解决共享内存区的竞争：
到此为止，我所给出的程序全部是不存在竞争关系的程序，一般来说很难有如此“和谐”的使用场景。那么，我们将之前的事件复杂化一些：
现在有多个进程在产生随机数，但只有一个进程来打印随机数，而我们不想让任何一个产生的随机数被“浪费”。
为了解决这样的问题，我们应当规划我们的共享内存区域，并且使用一个信号量保证不出现“浪费”：
*/
#include <semaphore.h>

void err(const char *str)
{
    perror(str);
    exit(EXIT_FAILURE);
}

// 共享内存区域
struct memory_area {
    sem_t sem;  // 信号量，用于同步和解决竞争
    int num;    // 存储产生的随机数
};

// 生产随机数函数
void produce(struct memory_area *p)
{
    for (srand(p->num); ; sleep(1)) {
        sem_wait(&(p->sem));
        int v = rand();
        p->num = v;
        printf("[%d] produce: %d\n", getpid(), v);
    }
}

// 打印随机数函数
void consume(struct memory_area *p)
{
    for (; ; sleep(1)) {
        printf("consume: %d\n", p->num);
        sem_post(&(p->sem));
    }
}

int main(int argc, const char *argv[]) {
    // 建立共享内存区域
    struct memory_area *p = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);

    if (p == MAP_FAILED) {
        err("mmap");
    }

    // 初始化信号量
    sem_init(&(p->sem), 1, 1);

    // 创建进程并分别执行打印和产生随机数的函数
    pid_t id1, id2;
    if ((id1 = fork()) != 0) {
        if ((id2 = fork()) != 0) {
            consume(p);
        } else {
            produce(p);
        }
    } else {
        produce(p);
    }

    // 销毁信号量
    sem_destroy(&(p->sem));
    // 解除映射
    if (munmap(p, sizeof(int)) == -1) {
        err("munmap");
    }

    return EXIT_SUCCESS;
}
/* 运行结果
warning: 'sem_init' is deprecated [-Wdeprecated-declarations]
warning: 'sem_destroy' is deprecated [-Wdeprecated-declarations]
2 warnings generated.
consume: 0
[4722] produce: 520932930
[4723] produce: 28925691
[4722] produce: 28925691
[4723] produce: 822784415
consume: 822784415
[4723] produce: 890459872
consume: 890459872
[4722] produce: 822784415
[4723] produce: 145532761
[4722] produce: 890459872
consume: 890459872
[4722] produce: 145532761
[4723] produce: 2132723841
consume: 2132723841
[4722] produce: 2132723841
[4723] produce: 1040043610
consume: 1040043610
[4722] produce: 1040043610
[4723] produce: 1643550337
consume: 1643550337
[4722] produce: 1643550337
[4723] produce: 68362598
consume: 68362598
[4722] produce: 68362598
[4723] produce: 66433441
consume: 66433441
[4722] produce: 66433441
[4723] produce: 2002830094
consume: 2002830094
[4722] produce: 2002830094
[4723] produce: 1906706780
consume: 1906706780
...
*/
#endif
