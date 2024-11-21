#include "../head/memory_paging.h"
#include <string.h>
#include <time.h>


// 初始化和生成访问序列
static void initialize_and_generate(MemoryManager *manager, int access_length) {
    // 初始化访问序列
    manager->access_seq.size = access_length;
    manager->access_seq.page_ids = malloc(access_length * sizeof(int));
    manager->access_seq.modes = malloc(access_length * sizeof(int));
    if (!manager->access_seq.page_ids || !manager->access_seq.modes) {
        fprintf(stderr, RED_COLOR "内存分配失败！\n" RESET_COLOR);
        exit(EXIT_FAILURE);
    }

    // 初始化内存管理器
    manager->pages = malloc(manager->total_pages * sizeof(Page));
    if (!manager->pages) {
        fprintf(stderr, RED_COLOR "内存分配失败！\n" RESET_COLOR);
        exit(EXIT_FAILURE);
    }
    manager->page_faults = 0;
    manager->total_accesses = 0;

    // 初始化页面属性
    for (int i = 0; i < manager->total_pages; i++) {
        manager->pages[i].id = i;
        double rand_val = (double)rand() / RAND_MAX;
        manager->pages[i].permissions = (rand_val > 0.7) ? READ_ONLY : READ_WRITE;
        manager->pages[i].accessed = NOT_ACCESSED;
        manager->pages[i].modified = NOT_MODIFIED;
    }

    // 生成访问序列
    int current_start = 0;
    for (int i = 0; i < access_length; i++) {
        int max_page = (current_start + 10 > manager->total_pages - 1) ? manager->total_pages - 1 : current_start + 10;
        manager->access_seq.page_ids[i] = rand() % (max_page - current_start + 1) + current_start;
        double mode = (double)rand() / RAND_MAX;
        if (mode > 0.7 && manager->pages[manager->access_seq.page_ids[i]].permissions == READ_WRITE)
            manager->access_seq.modes[i] = ACCESS_WRITE;
        else
            manager->access_seq.modes[i] = ACCESS_READ;

        // 20%概率改变起始位置
        if (((double)rand() / RAND_MAX) < 0.2)
            current_start = rand() % manager->total_pages;
        else
            current_start = (current_start + 1) % manager->total_pages;
    }
}

// 打印已加载的页面
static void print_loaded_pages(int *loaded, int size) {
    printf(YELLOW_COLOR "[");
    for (int j = 0; j < size; j++) {
        if (loaded[j] == -1)
            printf("空");
        else
            printf("%d", loaded[j]);
        if (j != size - 1)
            printf(", ");
    }
    printf("]" RESET_COLOR);
}

// 最佳置换算法
void optimal_replacement(MemoryManager *manager) {
    initialize_and_generate(manager, manager->access_seq.size);

    int loaded[3] = {-1, -1, -1};
    int loaded_count = 0;
    double total_time = 0.0;
    clock_t start_time, end_time;

    printf(BLUE_COLOR "=== 最佳置换算法 ===\n" RESET_COLOR);

    for (int i = 0; i < manager->access_seq.size; i++) {
        int current_page = manager->access_seq.page_ids[i];
        int access_mode = manager->access_seq.modes[i];
        printf("%s页面 %d...", (access_mode == ACCESS_READ) ? "读取" : "写入", current_page);

        // 检查页面是否已加载
        int loaded_index = -1;
        for (int j = 0; j < 3; j++) {
            if (loaded[j] == current_page) {
                loaded_index = j;
                break;
            }
        }

        if (loaded_count < 3) {
            if (loaded_index == -1) {
                loaded[loaded_count++] = current_page;
                manager->page_faults++;
                printf(GREEN_COLOR " 页面已加载至内存。" RESET_COLOR);
            }
            manager->total_accesses++;
            print_loaded_pages(loaded, 3);
            printf(" 预装阶段，不统计缺页次数。\n");
            continue;
        }

        if (loaded_index != -1) {
            manager->total_accesses++;
            double rate = (double)manager->page_faults / manager->total_accesses * 100;
            print_loaded_pages(loaded, 3);
            printf(GREEN_COLOR " 未发生缺页中断，当前缺页率：%.2lf%% (%d/%d)\n" RESET_COLOR, rate, manager->page_faults, manager->total_accesses);
        } else {
            start_time = clock();
            int replace_index = -1;
            int farthest = -1;
            for (int j = 0; j < 3; j++) {
                int next_use = -1;
                for (int k = i + 1; k < manager->access_seq.size; k++) {
                    if (manager->access_seq.page_ids[k] == loaded[j]) {
                        next_use = k;
                        break;
                    }
                }
                if (next_use == -1) { // 不再使用
                    replace_index = j;
                    break;
                }
                if (next_use > farthest) {
                    farthest = next_use;
                    replace_index = j;
                }
            }
            loaded[replace_index] = current_page;
            manager->page_faults++;
            manager->total_accesses++;
            end_time = clock();
            total_time += ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
            print_loaded_pages(loaded, 3);
            printf(RED_COLOR " 发生缺页中断，当前缺页率：%.2lf%% (%d/%d)\n" RESET_COLOR,
                   (double)manager->page_faults / manager->total_accesses * 100,
                   manager->page_faults, manager->total_accesses);
        }
    }
    printf("总耗时：%.6lf秒\n", total_time);
}

// 先进先出算法
void fifo_replacement(MemoryManager *manager) {
    initialize_and_generate(manager, manager->access_seq.size);

    int loaded[3] = {-1, -1, -1};
    int loaded_count = 0;
    int fifo_queue[3] = {0};
    int front = 0, rear = 0;
    double total_time = 0.0;
    clock_t start_time, end_time;

    printf(BLUE_COLOR "=== 先进先出算法 ===\n" RESET_COLOR);

    for (int i = 0; i < manager->access_seq.size; i++) {
        int current_page = manager->access_seq.page_ids[i];
        int access_mode = manager->access_seq.modes[i];
        printf("%s页面 %d...", (access_mode == ACCESS_READ) ? "读取" : "写入", current_page);

        // 检查页面是否已加载
        int loaded_index = -1;
        for (int j = 0; j < 3; j++) {
            if (loaded[j] == current_page) {
                loaded_index = j;
                break;
            }
        }

        if (loaded_count < 3) {
            if (loaded_index == -1) {
                loaded[loaded_count++] = current_page;
                fifo_queue[rear++] = current_page;
                manager->page_faults++;
                printf(GREEN_COLOR " 页面已加载至内存。" RESET_COLOR);
            }
            manager->total_accesses++;
            print_loaded_pages(loaded, 3);
            printf(" 预装阶段，不统计缺页次数。\n");
            continue;
        }

        if (loaded_index != -1) {
            manager->total_accesses++;
            double rate = (double)manager->page_faults / manager->total_accesses * 100;
            print_loaded_pages(loaded, 3);
            printf(GREEN_COLOR " 未发生缺页中断，当前缺页率：%.2lf%% (%d/%d)\n" RESET_COLOR, rate, manager->page_faults, manager->total_accesses);
        } else {
            start_time = clock();
            int replace_page = fifo_queue[front];
            front = (front + 1) % 3;
            // 替换页面
            for (int j = 0; j < 3; j++) {
                if (loaded[j] == replace_page) {
                    loaded[j] = current_page;
                    break;
                }
            }
            fifo_queue[rear] = current_page;
            rear = (rear + 1) % 3;
            manager->page_faults++;
            manager->total_accesses++;
            end_time = clock();
            total_time += ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
            print_loaded_pages(loaded, 3);
            printf(RED_COLOR " 发生缺页中断，当前缺页率：%.2lf%% (%d/%d)\n" RESET_COLOR,
                   (double)manager->page_faults / manager->total_accesses * 100,
                   manager->page_faults, manager->total_accesses);
        }
    }
    printf("总耗时：%.6lf秒\n", total_time);
}

// 最近最少使用算法
void lru_replacement(MemoryManager *manager) {
    initialize_and_generate(manager, manager->access_seq.size);

    int loaded[3] = {-1, -1, -1};
    int loaded_count = 0;
    int last_used[3] = {0};
    double total_time = 0.0;
    clock_t start_time, end_time;

    printf(BLUE_COLOR "=== 最近最少使用算法 ===\n" RESET_COLOR);

    for (int i = 0; i < manager->access_seq.size; i++) {
        int current_page = manager->access_seq.page_ids[i];
        int access_mode = manager->access_seq.modes[i];
        printf("%s页面 %d...", (access_mode == ACCESS_READ) ? "读取" : "写入", current_page);

        // 检查页面是否已加载
        int loaded_index = -1;
        for (int j = 0; j < 3; j++) {
            if (loaded[j] == current_page) {
                loaded_index = j;
                break;
            }
        }

        if (loaded_index != -1) {
            // 更新最近使用时间
            last_used[loaded_index] = i;
            manager->total_accesses++;
            double rate = (double)manager->page_faults / manager->total_accesses * 100;
            print_loaded_pages(loaded, 3);
            printf(GREEN_COLOR " 未发生缺页中断，当前缺页率：%.2lf%% (%d/%d)\n" RESET_COLOR, rate, manager->page_faults, manager->total_accesses);
        } else {
            start_time = clock();
            if (loaded_count < 3) {
                loaded[loaded_count] = current_page;
                last_used[loaded_count] = i;
                loaded_count++;
                printf(GREEN_COLOR " 页面已加载至内存。" RESET_COLOR);
            } else {
                // 找到最久未使用的页面
                int lru_index = 0;
                for (int j = 1; j < 3; j++) {
                    if (last_used[j] < last_used[lru_index])
                        lru_index = j;
                }
                loaded[lru_index] = current_page;
                last_used[lru_index] = i;
            }
            manager->page_faults++;
            manager->total_accesses++;
            end_time = clock();
            total_time += ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
            print_loaded_pages(loaded, 3);
            printf(RED_COLOR " 发生缺页中断，当前缺页率：%.2lf%% (%d/%d)\n" RESET_COLOR,
                   (double)manager->page_faults / manager->total_accesses * 100,
                   manager->page_faults, manager->total_accesses);
        }
    }
    printf("总耗时：%.6lf秒\n", total_time);
}

// 简单时钟算法
void clock_replacement(MemoryManager *manager) {
    initialize_and_generate(manager, manager->access_seq.size);

    Page loaded[3] = { {-1, 0, 0, 0}, {-1, 0, 0, 0}, {-1, 0, 0, 0} };
    int hand = 0;
    double total_time = 0.0;
    clock_t start_time, end_time;

    printf(BLUE_COLOR "=== 简单时钟算法 ===\n" RESET_COLOR);

    for (int i = 0; i < manager->access_seq.size; i++) {
        int current_page = manager->access_seq.page_ids[i];
        int access_mode = manager->access_seq.modes[i];
        printf("%s页面 %d...", (access_mode == ACCESS_READ) ? "读取" : "写入", current_page);

        // 检查页面是否已加载
        int found = -1;
        for (int j = 0; j < 3; j++) {
            if (loaded[j].id == current_page) {
                found = j;
                loaded[j].accessed = ACCESSED;
                if (access_mode == ACCESS_WRITE)
                    loaded[j].modified = MODIFIED;
                break;
            }
        }

        if (found != -1) {
            manager->total_accesses++;
            double rate = (double)manager->page_faults / manager->total_accesses * 100;
            print_loaded_pages((int[]){loaded[0].id, loaded[1].id, loaded[2].id}, 3);
            printf(GREEN_COLOR " 未发生缺页中断，当前缺页率：%.2lf%% (%d/%d)\n" RESET_COLOR, rate, manager->page_faults, manager->total_accesses);
            continue;
        }

        // 页面缺失，进行替换
        start_time = clock();
        while (1) {
            if (loaded[hand].accessed == NOT_ACCESSED) {
                loaded[hand].id = current_page;
                loaded[hand].accessed = ACCESSED;
                loaded[hand].modified = (access_mode == ACCESS_WRITE) ? MODIFIED : NOT_MODIFIED;
                hand = (hand + 1) % 3;
                break;
            } else {
                loaded[hand].accessed = NOT_ACCESSED;
                hand = (hand + 1) % 3;
            }
        }
        manager->page_faults++;
        manager->total_accesses++;
        end_time = clock();
        total_time += ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
        print_loaded_pages((int[]){loaded[0].id, loaded[1].id, loaded[2].id}, 3);
        printf(RED_COLOR " 发生缺页中断，当前缺页率：%.2lf%% (%d/%d)\n" RESET_COLOR,
               (double)manager->page_faults / manager->total_accesses * 100,
               manager->page_faults, manager->total_accesses);
    }
    printf("总耗时：%.6lf秒\n", total_time);
}

// 改进型时钟算法
void enhanced_clock_replacement(MemoryManager *manager) {
    initialize_and_generate(manager, manager->access_seq.size);

    Page loaded[3] = { {-1, 0, 0, 0}, {-1, 0, 0, 0}, {-1, 0, 0, 0} };
    int hand = 0;
    double total_time = 0.0;
    clock_t start_time, end_time;

    printf(BLUE_COLOR "=== 改进型时钟算法 ===\n" RESET_COLOR);

    for (int i = 0; i < manager->access_seq.size; i++) {
        int current_page = manager->access_seq.page_ids[i];
        int access_mode = manager->access_seq.modes[i];
        printf("%s页面 %d...", (access_mode == ACCESS_READ) ? "读取" : "写入", current_page);

        // 检查页面是否已加载
        int found = -1;
        for (int j = 0; j < 3; j++) {
            if (loaded[j].id == current_page) {
                found = j;
                loaded[j].accessed = ACCESSED;
                if (access_mode == ACCESS_WRITE)
                    loaded[j].modified = MODIFIED;
                break;
            }
        }

        if (found != -1) {
            manager->total_accesses++;
            double rate = (double)manager->page_faults / manager->total_accesses * 100;
            print_loaded_pages((int[]){loaded[0].id, loaded[1].id, loaded[2].id}, 3);
            printf(GREEN_COLOR " 未发生缺页中断，当前缺页率：%.2lf%% (%d/%d)\n" RESET_COLOR, rate, manager->page_faults, manager->total_accesses);
            continue;
        }

        // 页面缺失，进行替换
        start_time = clock();
        int replaced = 0;
        for (int pass = 0; pass < 2; pass++) {
            for (int j = 0; j < 3; j++) {
                if (loaded[hand].accessed == NOT_ACCESSED && loaded[hand].modified == NOT_MODIFIED) {
                    loaded[hand].id = current_page;
                    loaded[hand].accessed = ACCESSED;
                    loaded[hand].modified = (access_mode == ACCESS_WRITE) ? MODIFIED : NOT_MODIFIED;
                    hand = (hand + 1) % 3;
                    replaced = 1;
                    break;
                }
                loaded[hand].accessed = NOT_ACCESSED;
                hand = (hand + 1) % 3;
            }
            if (replaced)
                break;
        }
        // 如果还未替换，则替换当前指针指向的页面
        if (!replaced) {
            loaded[hand].id = current_page;
            loaded[hand].accessed = ACCESSED;
            loaded[hand].modified = (access_mode == ACCESS_WRITE) ? MODIFIED : NOT_MODIFIED;
            hand = (hand + 1) % 3;
        }
        manager->page_faults++;
        manager->total_accesses++;
        end_time = clock();
        total_time += ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
        print_loaded_pages((int[]){loaded[0].id, loaded[1].id, loaded[2].id}, 3);
        printf(RED_COLOR " 发生缺页中断，当前缺页率：%.2lf%% (%d/%d)\n" RESET_COLOR,
               (double)manager->page_faults / manager->total_accesses * 100,
               manager->page_faults, manager->total_accesses);
    }
    printf("总耗时：%.6lf秒\n", total_time);
}