#ifndef UNIQUE_QUEUE_H
#define UNIQUE_QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// 定义哈希表节点结构体
typedef struct HashNode {
    int key;
    struct HashNode *next;
} HashNode;

// 定义哈希表结构体
typedef struct HashTable {
    HashNode **table;
    int size;
} HashTable;

// 创建一个空的哈希表
HashTable *createHashTable(int size);

// 散列函数
int hashFunction(int key, int size);

// 插入元素到哈希表
void insertHashTable(HashTable *ht, int key);

// 查找元素是否存在于哈希表中
bool contains(HashTable *ht, int key);

// 删除哈希表
void deleteHashTable(HashTable *ht);

// 定义队列节点结构体
typedef struct QueueNode {
    int data;
    struct QueueNode *next;
} Node;

// 定义队列结构体
typedef struct UniqueQueue {
    Node *front;
    Node *rear;
    HashTable *hashTable;
} UniqueQueue;

// 创建一个空队列
UniqueQueue *createUniqueQueue(int hashSize);

// 入队操作（去重）
void enqueueUnique(UniqueQueue *q, int value);

// 出队操作
int dequeue(UniqueQueue *q);

// 检查队列是否为空
bool is_empty(UniqueQueue *q);

// 打印队列中的所有元素
void printQueue(UniqueQueue *q);

// 释放队列资源
void destroyQueue(UniqueQueue *q);

#endif // UNIQUE_QUEUE_H