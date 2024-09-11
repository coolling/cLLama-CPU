#include "unique_queue.h"

// 创建一个空的哈希表
HashTable *createHashTable(int size) {
    HashTable *ht = (HashTable *)malloc(sizeof(HashTable));
    ht->table = (HashNode **)calloc(size, sizeof(HashNode *));
    ht->size = size;
    return ht;
}

// 散列函数
int hashFunction(int key, int size) {
    return key % size;
}

// 插入元素到哈希表
void insertHashTable(HashTable *ht, int key) {
    int index = hashFunction(key, ht->size);
    HashNode *newNode = (HashNode *)malloc(sizeof(HashNode));
    newNode->key = key;
    newNode->next = ht->table[index];
    ht->table[index] = newNode;
}

// 查找元素是否存在于哈希表中
bool contains(HashTable *ht, int key) {
    int index = hashFunction(key, ht->size);
    HashNode *current = ht->table[index];
    while (current != NULL) {
        if (current->key == key) {
            return true;
        }
        current = current->next;
    }
    return false;
}

// 删除哈希表
void deleteHashTable(HashTable *ht) {
    for (int i = 0; i < ht->size; i++) {
        HashNode *current = ht->table[i];
        HashNode *prev = NULL;
        while (current != NULL) {
            HashNode *next = current->next;
            free(current);
            current = next;
        }
    }
    free(ht->table);
    free(ht);
}

// 创建一个空队列
UniqueQueue *createUniqueQueue(int hashSize) {
    UniqueQueue *q = (UniqueQueue *)malloc(sizeof(UniqueQueue));
    q->front = NULL;
    q->rear = NULL;
    q->hashTable = createHashTable(hashSize);
    return q;
}

// 入队操作（去重）
void enqueueUnique(UniqueQueue *q, int value) {
    if (!contains(q->hashTable, value)) {
        Node *newNode = (Node *)malloc(sizeof(Node));
        newNode->data = value;
        newNode->next = NULL;

        if (q->rear == NULL) { // 如果队列为空
            q->front = newNode;
            q->rear = newNode;
        } else {
            q->rear->next = newNode;
            q->rear = newNode;
        }

        insertHashTable(q->hashTable, value);
    }
}

// 出队操作
int dequeue(UniqueQueue *q) {
    if (q->front == NULL) {
        printf("Queue is empty!\n");
        return -1;
    }
    Node *temp = q->front;
    int value = temp->data;
    q->front = q->front->next;

    if (q->front == NULL) { // 如果队列现在为空
        q->rear = NULL;
    }

    free(temp);
    return value;
}

// 检查队列是否为空
bool is_empty(UniqueQueue *q) {
    return q->front == NULL;
}

// 打印队列中的所有元素
void printQueue(UniqueQueue *q) {
    Node *current = q->front;
    while (current != NULL) {
        printf("%d ", current->data);
        current = current->next;
    }
    printf("\n");
}

// 释放队列资源
void destroyQueue(UniqueQueue *q) {
    while (!is_empty(q)) {
        dequeue(q);
    }
    deleteHashTable(q->hashTable);
    free(q);
}