#include "coolling-util/CpuMonitor.h"
// 查看空闲核心
// 查看空闲核心
hhh
int main() {
    clock_t start, end;
    start = clock();
    getIdleCoresCount();
    end = clock();
    double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("CPU time used: %f seconds\n", cpu_time_used);
    return EXIT_SUCCESS;
}