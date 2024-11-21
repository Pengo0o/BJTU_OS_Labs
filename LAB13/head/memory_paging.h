#ifndef MEMORY_PAGING_H
#define MEMORY_PAGING_H

#include <stdio.h>
#include <stdlib.h>


// 颜色定义
#define RESET_COLOR "\033[0m"
#define RED_COLOR "\033[31m"
#define GREEN_COLOR "\033[32m"
#define YELLOW_COLOR "\033[33m"
#define BLUE_COLOR "\033[34m"

// 页面权限定义
#define READ_ONLY 0
#define READ_WRITE 1

// 访问标志定义
#define NOT_ACCESSED 0
#define ACCESSED 1

// 修改标志定义
#define NOT_MODIFIED 0
#define MODIFIED 1

typedef struct {
    int id;          // 页面编号
    int permissions; // 读写权限
    int accessed;    // 访问标志
    int modified;    // 修改标志
} Page;

#define ACCESS_READ 1
#define ACCESS_WRITE 2

typedef struct {
    int size;        // 访问列表长度
    int *page_ids;   // 页面编号数组
    int *modes;      // 访问模式数组
} AccessSequence;

typedef struct {
    int total_pages;          // 总页面数
    Page *pages;              // 页面数组
    AccessSequence access_seq; // 访问序列
    int page_faults;          // 缺页次数
    int total_accesses;       // 总访问次数
} MemoryManager;

// 页面置换算法函数声明
void optimal_replacement(MemoryManager *manager);
void fifo_replacement(MemoryManager *manager);
void lru_replacement(MemoryManager *manager);
void clock_replacement(MemoryManager *manager);
void enhanced_clock_replacement(MemoryManager *manager);

#endif // MEMORY_PAGING_H