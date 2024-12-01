// CpuMonitor.c
#include "CpuMonitor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sched.h>
#define BUFFER_SIZE 128
#define THRESHOLD 99.0

int getAllCores(){
    return  sysconf(_SC_NPROCESSORS_ONLN);
}
int countIdleCores(const char *output,Core *cores,int size) {
    int idleCount = 0;
    char line[BUFFER_SIZE];
    char *token;
    double idleRate;

    // 逐行处理输出
    const char *ptr = output;
    int flag=0;
    int count=0;
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
                    if(!cores[count].isUsed){
                        idleCount++;

                    }
                    
                    cores[count].isIdle=true;
                }else{
                    cores[count].isIdle=false;

                }
            }
            count++;
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
int getIdleCoresCount(Core * cores,int size){
    const char *cmd = "mpstat -P ALL 1 1";
    char *mpstatOutput = executeCommand(cmd);
    if (mpstatOutput == NULL) {
        return EXIT_FAILURE;
    }

    int idleCores = countIdleCores(mpstatOutput,cores,size);
    // printf("空闲核心的数量: %d\n", idleCores);

    free(mpstatOutput); // 释放内存
    return idleCores;
}




// 将16进制字符串转换为二进制字符串
void hexToBinary(const char *hexStr, char *binaryStr) {
    int i, j;
    for (i = 0, j = 0; hexStr[i] != '\0'; i++) {
        // 跳过逗号
        if (hexStr[i] == ',') continue;
        if (hexStr[i] == '\n') continue;

        int hexValue = 0;
        // 将单个16进制数字转换为4位二进制数字
        if (hexStr[i] >= '0' && hexStr[i] <= '9') {
            hexValue = hexStr[i] - '0';
        } else if (hexStr[i] >= 'A' && hexStr[i] <= 'F') {
            hexValue = 10 + (hexStr[i] - 'A');
        } else if (hexStr[i] >= 'a' && hexStr[i] <= 'f') {
            hexValue = 10 + (hexStr[i] - 'a');
        }

        // 将4位二进制数字填充到字符串中
        for (int k = 3; k >= 0; k--) {
            binaryStr[j++] = (hexValue & (1 << k)) ? '1' : '0';
        }
    }
    binaryStr[j] = '\0'; // 字符串结束符
}

// 从右到左统计1的索引
void countOnesIndex(const char *binaryStr, int *indexArray, int *indexCount) {
    int length = strlen(binaryStr);
    for (int i = length - 1; i >= 0; i--) {
        if (binaryStr[i] == '1') {
            indexArray[(*indexCount)++] = length - i - 1;
        }
    }
}




// 初始化核心
void init_cores(Core **cores,int size, SharedGroup **shared_groups_level2, int count2, SharedGroup **shared_groups_level3, int count3) {
    *cores = (Core *)malloc(size * sizeof(Core));
    for (int i = 0; i < size; i++) {
        (*cores)[i].id =i;
        (*cores)[i].last_assigned_time = 0;
        (*cores)[i].isUsed = false;
        (*cores)[i].isIdle = true;
        (*cores)[i].shared_group_level2 = NULL;
        (*cores)[i].shared_group_level3 = NULL;
        (*cores)[i].count2=0;
        (*cores)[i].count3=0;

    }
    bool isvisited[size];
    memset(isvisited, false, sizeof(isvisited));
    // 将共享组关联到每个核心
    for (int i = 0; i < size; i++) {
        if (isvisited[i] == false) {
                isvisited[i] = true;
        } else {
                continue;
        }
        for (int j = 0; j < count2; j++) {
            for (int k = 0; k < (*shared_groups_level2)[j].count; k++) {       
                    
                (*cores)[(*shared_groups_level2)[j].cores[k]].shared_group_level2 = &(*shared_groups_level2)[j];
                (*cores)[(*shared_groups_level2)[j].cores[k]].count2=(*shared_groups_level2)[j].count;
                isvisited[(*shared_groups_level2)[j].cores[k]]=true;
            }
        }

        
    }
    
    memset(isvisited, false, sizeof(isvisited));
    // 将共享组关联到每个核心
    for (int i = 0; i < size; i++) {
        if (isvisited[i] == false) {
                isvisited[i] = true;
        } else {
                continue;
        }
        for (int j = 0; j < count3; j++) {
            for (int k = 0; k < (*shared_groups_level3)[j].count; k++) {
             
                
                (*cores)[(*shared_groups_level3)[j].cores[k]].count3=(*shared_groups_level3)[j].count;
                (*cores)[(*shared_groups_level3)[j].cores[k]].shared_group_level3 = &((*shared_groups_level3)[j]);
                isvisited[(*shared_groups_level3)[j].cores[k]]=true;
            }
        }
    }
  
}



// 初始化共享组
void init_shared_groups(int size, SharedGroup **shared_groups_level2,int * countl2, SharedGroup **shared_groups_level3,int * countl3) {
    char src[100];
    char hexStr[100]; // 假设输入的字符串不超过100个字符
    char binaryStr[100]; // 存储转换后的二进制字符串
    int cores = size;
    int count2 = 0;
    int count3 = 0;

    // 临时存储共享组信息
    SharedGroup *temp_shared_groups_level2 = NULL;
    SharedGroup *temp_shared_groups_level3 = NULL;

    temp_shared_groups_level2 = (SharedGroup *)malloc(size * sizeof(SharedGroup));
    temp_shared_groups_level3 = (SharedGroup *)malloc(size * sizeof(SharedGroup));


    // 计算共享组大小并保存核心索引
    for (int index = 2; index <= 3; index++) {
        bool isvisited[cores];
        memset(isvisited, false, sizeof(isvisited));
        for (int i = 0; i < cores; i++) {
            if (isvisited[i] == false) {
                isvisited[i] = true;
            } else {
                continue;
            }

            // 构造文件路径
            sprintf(src, "/sys/devices/system/cpu/cpu%d/cache/index%d/shared_cpu_map", i, index);

            // 打开文件
            FILE *file = fopen(src, "r");
            if (file == NULL) {
                perror("Error opening file");
                continue; // 如果文件打开失败，跳过当前迭代
            }

            // 读取文件内容
            fgets(hexStr, 100, file);

            // 转换为二进制
            hexToBinary(hexStr, binaryStr);

            // 统计1的索引
            int indexCount = 0;
            int *indexArray = malloc(size * sizeof(int));
            if (indexArray == NULL) {
                fprintf(stderr, "Memory allocation failed for indexArray!\n");
                exit(EXIT_FAILURE);
            }
            countOnesIndex(binaryStr, indexArray, &indexCount);
            // 打印结果
            
            for (int j = 0;j < indexCount; j++) {
                
                isvisited[indexArray[j]] = true;
            }

            if (index == 2) {
                SharedGroup * ts= &temp_shared_groups_level2[count2];
                ts->count = indexCount;
                ts->cores = indexArray;
                count2++;
            } else {
                SharedGroup * ts= &temp_shared_groups_level3[count3];
                ts->count = indexCount;
                ts->cores = indexArray;
                count3++;
            }

            // 关闭文件
            fclose(file);
        }
    }
    printf("%d %d\n",count2,count3);
    // 动态分配内存
    *shared_groups_level2 = (SharedGroup *)malloc(count2 * sizeof(SharedGroup));
    *shared_groups_level3 = (SharedGroup *)malloc(count3 * sizeof(SharedGroup));

    if (!(*shared_groups_level2) || !(*shared_groups_level3)) {
        fprintf(stderr, "Memory allocation failed for final shared groups!\n");
        exit(EXIT_FAILURE);
    }

    // 赋值
    for (int i = 0; i < count2; i++) {
      
        (*shared_groups_level2)[i].count = temp_shared_groups_level2[i].count;
        (*shared_groups_level2)[i].cores = (int *)malloc(temp_shared_groups_level2[i].count * sizeof(int));
        if ((*shared_groups_level2)[i].cores == NULL) {
            fprintf(stderr, "Memory allocation failed for cores in level2 shared group!\n");
            exit(EXIT_FAILURE);
        }
        memcpy((*shared_groups_level2)[i].cores, temp_shared_groups_level2[i].cores, temp_shared_groups_level2[i].count * sizeof(int));
    }

    for (int i = 0; i < count3; i++) {
       
        (*shared_groups_level3)[i].count = temp_shared_groups_level3[i].count;
        (*shared_groups_level3)[i].cores = (int *)malloc(temp_shared_groups_level3[i].count * sizeof(int));
        if ((*shared_groups_level3)[i].cores == NULL) {
            fprintf(stderr, "Memory allocation failed for cores in level3 shared group!\n");
            exit(EXIT_FAILURE);
        }
        memcpy((*shared_groups_level3)[i].cores, temp_shared_groups_level3[i].cores, temp_shared_groups_level3[i].count * sizeof(int));
    }

    // // 打印结果
    // for (int j = 0; j < count2; j++) {
    //     printf("Level 2 Group %d: Cores ", j);
    //     for (int z = 0; z < (*shared_groups_level2)[j].count; z++) {
    //         printf("%d ", (*shared_groups_level2)[j].cores[z]);
    //     }
    //     printf("\n");
    // }

    // for (int j = 0; j < count3; j++) {
    //     printf("Level 3 Group %d: Cores ", j);
    //     for (int z = 0; z < (*shared_groups_level3)[j].count; z++) {
    //         printf("%d ", (*shared_groups_level3)[j].cores[z]);
    //     }
    //     printf("\n");
    // }

    // 释放临时内存
    for (int i = 0; i < count2; i++) {
        free(temp_shared_groups_level2[i].cores);
    }
    for (int i = 0; i < count3; i++) {
        free(temp_shared_groups_level3[i].cores);
    }
    free(temp_shared_groups_level2);
    free(temp_shared_groups_level3);
    *countl2=count2;
    *countl3=count3;
}


// 查找空闲核心
Core *find_idle_core(Core *cores, int core_count) {
    Core *closest_core = NULL;
    time_t closest_time =0; 
    int index =-1;
    for (int i = 0; i < core_count; i++) {
        if (cores[i].isUsed == false) { // 如果核心为空闲
            if (closest_core == NULL || cores[i].last_assigned_time>closest_time) {
                closest_core = &cores[i];
                closest_time = cores[i].last_assigned_time;
                index =i;
            }
        }
    }
    
    return closest_core;
}




// 更新核心的最晚分配时间
void update_last_assigned_time(Core *cores,int core_id) {
    cores[core_id].last_assigned_time = time(NULL);
    cores[core_id].isUsed=false;
}

int find_core_assigned(Core *cores, int core_count) {
    Core shared_idles[core_count];
    bool isvisited[core_count];
    int count=0;
    memset(isvisited, false, sizeof(isvisited));
    for(int i=0;i<core_count;i++){
            if(cores[i].isUsed==true){
            //    printf("id:%d\nshared_group_level2:\n",cores[i].id);
               
                for(int j=0;j<cores[i].count2;j++){
                    
                    if(!cores[cores[i].shared_group_level2->cores[j]].isUsed&&!isvisited[cores[cores[i].shared_group_level2->cores[j]].id]&&cores[cores[i].shared_group_level2->cores[j]].isIdle){
                        shared_idles[count++]=cores[cores[i].shared_group_level2->cores[j]];
                        isvisited[cores[cores[i].shared_group_level2->cores[j]].id]=true;
                    }
                }
            }
    }
    // printf("count!!!! %d\n",count);
    if(count!=0){
        Core * re=find_idle_core(shared_idles,count);
        // printf("bind %d\n",re->id);
        return re->id;
        // bind_thread_to_core(re->id,cores);
        
    }else{
        memset(isvisited, false, sizeof(isvisited));
        for(int i=0;i<core_count;i++){
            if(cores[i].isUsed==true){
             
                for(int j=0;j<cores[i].count3;j++){
                    
                    if(!cores[cores[i].shared_group_level3->cores[j]].isUsed&&!isvisited[cores[cores[i].shared_group_level3->cores[j]].id]&&cores[cores[i].shared_group_level3->cores[j]].isIdle){
                        shared_idles[count++]=cores[cores[i].shared_group_level3->cores[j]];
                        isvisited[cores[cores[i].shared_group_level3->cores[j]].id]=true;
                    }
                }
            }
        }
        Core * re=find_idle_core(shared_idles,count);
        // printf("count!!!! %d\n",count);
        if(count!=0){
            
            // printf("bind %d\n",re->id);
            return re->id;
        }else{
            for(int i=0;i<core_count;i++){
                if(cores[i].isUsed==false){
                    return i;
                }
            }
            return -1;
        }

    }
    return -1;
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

// 函数定义
long getFreeMemoryBytes() {
    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        // 返回空闲内存量，单位为字节
        return info.freeram/1024/1024 -88*1024 ;
    } else {
        // 如果sysinfo失败，返回-1表示错误
        perror("Error getting system information");
        return -1;
    }
}