#include "../head/memory_paging.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// 选择置换算法
typedef enum {
    OPTIMAL = 1,
    FIFO,
    LRU,
    CLOCK,
    ENHANCED_CLOCK
} ReplacementAlgorithm;

// 显示菜单
void display_menu() {
    printf(BLUE_COLOR "请选择页面置换算法:\n" RESET_COLOR);
    printf("1. 最佳置换算法 (OPT)\n");
    printf("2. 先进先出算法 (FIFO)\n");
    printf("3. 最近最少使用算法 (LRU)\n");
    printf("4. 简单时钟算法 (Clock)\n");
    printf("5. 改进型时钟算法 (Enhanced Clock)\n");
    printf("请输入选项(1-5): ");
}

int main() {
    srand((unsigned int)time(NULL));
    printf(BLUE_COLOR "===22331170 Huan Yunpeng置换算法模拟 ===\n" RESET_COLOR);
    int total_pages = 20;
    int access_length = 40;
    MemoryManager manager;
    manager.total_pages = total_pages;
    manager.access_seq.size = access_length; 

    int choice;
    display_menu();
    scanf("%d", &choice);

    switch(choice) {
        case OPTIMAL:
            optimal_replacement(&manager);
            break;
        case FIFO:
            fifo_replacement(&manager);
            break;
        case LRU:
            lru_replacement(&manager);
            break;
        case CLOCK:
            clock_replacement(&manager);
            break;
        case ENHANCED_CLOCK:
            enhanced_clock_replacement(&manager);
            break;
        default:
            printf(RED_COLOR "无效的选择！\n" RESET_COLOR);
            return EXIT_FAILURE;
    }

    // 释放内存
    if (manager.pages)
        free(manager.pages);
    if (manager.access_seq.page_ids)
        free(manager.access_seq.page_ids);
    if (manager.access_seq.modes)
        free(manager.access_seq.modes);

    return EXIT_SUCCESS;
}

