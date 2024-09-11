// CpuMonitor.c
#include "CpuMonitor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sched.h>
#define BUFFER_SIZE 128
#define THRESHOLD 93.0

int getAllCores(){
    return  sysconf(_SC_NPROCESSORS_ONLN);
}
int countIdleCores(const char *output) {
    int idleCount = 0;
    char line[BUFFER_SIZE];
    char *token;
    double idleRate;

    // 逐行处理输出
    const char *ptr = output;
    int flag=0;
    while (sscanf(ptr, "%[^\n]\n", line) != EOF) {
        ptr += strlen(line) + 1; // 移动到下一行
        // 查找包含 "PM" 的行
        if (strstr(line, "PM") != NULL ||strstr(line, "AM") != NULL) {
            if(flag==0){
                flag=1;
                continue;
            }
       
            // 使用 sscanf 提取空闲率
            token = strtok(line, " ");
            for (int i = 0; i < 12; i++) { // 跳过前 12 个字段
                token = strtok(NULL, " ");
            }
            if (token != NULL) {
                idleRate = atof(token);
                if (idleRate > THRESHOLD) {
                   
                    idleCount++;
                }
            }
        }
    }

    return idleCount;
}

char* executeCommand(const char *cmd) {
    FILE *fp;
    char *output = malloc(BUFFER_SIZE * 100); // 假设最多 100 行
    char buffer[BUFFER_SIZE];
    output[0] = '\0'; // 初始化为空

    // 执行命令
    fp = popen(cmd, "r");
    if (fp == NULL) {
        perror("popen failed");
        return NULL;
    }

    // 读取命令输出
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        strcat(output, buffer);
    }

    pclose(fp);
    return output;
}
int getIdleCoresCount(){
    const char *cmd = "mpstat -P ALL 1 1";
    char *mpstatOutput = executeCommand(cmd);
    if (mpstatOutput == NULL) {
        return EXIT_FAILURE;
    }

    int idleCores = countIdleCores(mpstatOutput);
    // printf("空闲核心的数量: %d\n", idleCores);

    free(mpstatOutput); // 释放内存
    return idleCores;
}
// 函数用于将当前线程绑定到指定的核心
// int bind_thread_to_core(int core_id) {
//     cpu_set_t *cpuset;
//     size_t setsize;

//     // 获取系统中可用的 CPU 核心数
//     int ncpu = sysconf(_SC_NPROCESSORS_ONLN);
//     printf("cpu count:%d\n",ncpu);
//     if (ncpu < 0) {
//         perror("Error getting the number of online processors");
//         return -1;
//     }

//     // 获取分配给 cpu_set_t 的大小
//     setsize = CPU_ALLOC_SIZE(ncpu);
//     cpuset = CPU_ALLOC(ncpu);
//     if (cpuset == NULL) {
//         perror("Error allocating CPU set");
//         return -1;
//     }

//     // 初始化 CPU 集合并设置指定的核心
//     CPU_ZERO_S(setsize, cpuset);
//     if (CPU_SET_S(core_id, setsize, cpuset) == 0) {
//         perror("Error setting CPU");
//         CPU_FREE(cpuset);
//         return -1;
//     }
//     // 将当前线程绑定到指定的核心
//     int result = pthread_setaffinity_np(pthread_self(), setsize, cpuset);
//     if (result != 0) {
//         fprintf(stderr, "Error binding thread to core %d: %s\n", core_id, strerror(result));
//         CPU_FREE(cpuset);
//         return -1;
//     }

//     // 打印绑定结果
//     printf("Thread bound to core %d\n", core_id);

//     // 释放CPU集合
//     CPU_FREE(cpuset);
//     return 0;
// }
// 查看空闲核心
// int main() {
//     clock_t start, end;
//     start = clock();
//     getIdleCoresCount();
//     end = clock();
//     double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
//     printf("CPU time used: %f seconds\n", cpu_time_used);
//     return EXIT_SUCCESS;
// }

// int main() {
    
// 线程绑定核心
//     printf("Current thread is running on CPU core %d\n", sched_getcpu());
//     int core_id = 5; // 假设我们要将线程绑定到核心5
//     if (bind_thread_to_core(core_id) != 0) {
//         fprintf(stderr, "Failed to bind thread to core %d\n", core_id);
//         return -1;
//     }
//     int cpu_id = sched_getcpu();
//     if (cpu_id < 0) {
//         perror("sched_getcpu failed");
//         return -1;
//     }
//     printf("Current thread is running on CPU core %d\n", sched_getcpu());
    // 线程将继续在核心5上执行
//     return 0;
// }