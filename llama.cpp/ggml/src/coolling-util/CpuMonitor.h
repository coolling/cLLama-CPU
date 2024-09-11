// CpuMonitor.h
#ifndef CPUMONITOR_H
#define CPUMONITOR_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sched.h>

#define BUFFER_SIZE 128
#define THRESHOLD 93.0

// 声明 executeCommand 函数，它将执行一个命令并返回命令的输出
char* executeCommand(const char *cmd);

// 声明 countIdleCores 函数，它将计算空闲核心的数量
int countIdleCores(const char *output);

// 声明 getIdleCoresCount 函数，它将执行 mpstat 命令并返回空闲核心的数量
int getIdleCoresCount(void);

// 声明 bind_thread_to_core函数，用于将当前线程绑定到指定的核心
int bind_thread_to_core(int core_id) ;

int getAllCores(void);

#endif // CPUMONITOR_H