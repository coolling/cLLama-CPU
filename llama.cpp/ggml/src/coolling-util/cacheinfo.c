#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#define BUFFER_SIZE 100

// 函数声明
void hexToBinary(const char *hexStr, char *binaryStr);
void countOnesIndex(const char *binaryStr, int *indexArray, int *indexCount);
bool cacheSharing[2][40][40];
int main() {
    char src[100];
    
    char hexStr[100]; // 假设输入的字符串不超过100个字符
    char binaryStr[100]; // 存储转换后的二进制字符串
    int indexArray[100]; // 存储1的索引位置
    int cores=40;
    
    for(int index =2;index<=3;index++){
        bool isvisited[cores];
        memset(isvisited, false, sizeof(isvisited));
        for (int i = 0; i <cores; i++) {
            if(isvisited[i]==false){
                isvisited[i]=true;
            }else{
                continue;
            }
            int indexCount = 0; // 1的索引位置数量
            // 构造文件路径
            sprintf(src, "/sys/devices/system/cpu/cpu%d/cache/index%d/shared_cpu_map", i,index);

            // 打开文件
            FILE *file = fopen(src, "r");
            if (file == NULL) {
                perror("Error opening file");
                continue; // 如果文件打开失败，跳过当前迭代
            }

            // 读取文件内容
            fgets(hexStr, BUFFER_SIZE, file);
            // 转换为二进制
           
            hexToBinary(hexStr, binaryStr);
          

            // 统计1的索引
            countOnesIndex(binaryStr, indexArray, &indexCount);

            // 打印结果
            for (int j = 0;j < indexCount; j++) {
                for(int z=0;z<indexCount;z++){
                    // if(j!=z){
                        cacheSharing[index-2][indexArray[j]][indexArray[z]]=true;
                    // }
                }
                
            }
            
         
            // 关闭文件
            fclose(file);
            memset(indexArray, 0, sizeof(indexArray));
        }
        printf("\n");
        for(int j=0;j<cores;j++){
            for(int z=0;z<cores;z++){
                printf("%d ",cacheSharing[index-2][j][z]);
            }
            printf("\n");
        }

    }
    

    return 0;
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