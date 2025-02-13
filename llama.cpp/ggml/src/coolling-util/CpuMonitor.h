// CpuMonitor.h
#ifndef CPUMONITOR_H
#define CPUMONITOR_H
#define __USE_GNU

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sched.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#define BUFFER_SIZE 128


// 定义 CPU 核心结构体
typedef struct Core {
    int id;
    time_t last_assigned_time; // 最近一次分配的时间
    bool isUsed;     
    bool isIdle;  
    struct SharedGroup *shared_group_level2; // 共享组指针
    int count2;
    struct SharedGroup *shared_group_level3; // 共享组指针
    int count3;
} Core;

// 定义共享组结构体
typedef struct SharedGroup {
    int *cores; // 共享组中的核心数组
    int count;          // 共享组中的核心数量
} SharedGroup;
// 声明 executeCommand 函数，它将执行一个命令并返回命令的输出
char* executeCommand(const char *cmd);
// 声明 countIdleCores 函数，它将计算空闲核心的数量
int countIdleCores(const char *output,Core * cores,int size);
// 声明 getIdleCoresCount 函数，它将执行 mpstat 命令并返回空闲核心的数量
int getIdleCoresCount(Core * cores,int size);
int getAllCores(void);

// 函数声明
void hexToBinary(const char *hexStr, char *binaryStr);
void countOnesIndex(const char *binaryStr, int *indexArray, int *indexCount);
void init_cores(Core **cores,int size, SharedGroup **shared_groups_level2, int count2, SharedGroup **shared_groups_level3, int count3);
void init_shared_groups(int size, SharedGroup **shared_groups_level2, int *count2, SharedGroup **shared_groups_level3, int *count3);
Core *find_idle_core(Core *cores, int core_count);
int bind_thread_to_core(int core_id, Core *cores);
void update_last_assigned_time(Core *cores,int core_id) ;
int find_core_assigned(Core *cores, int core_count);
// int find_core_assigned(Core *cores, int core_count,int target_count,int* result);
#endif // CPUMONITOR_H

typedef struct {
    int thread_id;
    size_t offset;
    size_t size;
    void *buffer;
} thread_args; //用于读取数据的线程
// 链表节点
typedef struct Node {
    void* data;
    size_t offset;
    size_t size;
    struct Node *next;
} Node;


// 函数声明
int getFreeMemoryBytes();


extern  char* FILENAME ; //加载的模型地址


#define  NUM_THREADS 4 //读取的线程数
extern int NUM_CORES ; //读取的线程数
extern int RESERVE_CORES  ;//预留的核心数
extern int RESERVE_MEM ;//预留的内存数
extern float THRESHOLD  ;//空闲核心的阈值
extern char* CURRENT_MEM_FILE ;
extern char* MAX_MEM_FILE;