#include "/opt/data/private/hyp/BJTU_OS_Labs/LAB10/head/bank.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    printf(BOLD BLUE "22331170-YunpengHuan-banks\n" RESET);

    const int num_processes = 5;
    const int num_resources = 4;

    Process p[num_processes];
    for (int i = 0; i < num_processes; i++) {
        init_process(&p[i], num_resources);
    }

    // Set up processes
    int max_resources[5][4] = {
    {6, 4, 7, 3},  
    {4, 2, 3, 2},  
    {2, 5, 3, 3},  
    {6, 3, 3, 2},  
    {5, 6, 7, 5}   
    };

    int allocations[5][4] = {
    {4, 2, 3, 1},  
    {2, 0, 1, 1},  
    {0, 2, 1, 1},  
    {2, 1, 1, 1},  
    {1, 3, 5, 2}   
    };

    int needs[5][3];
    for (int i = 0; i < num_processes; i++) {
        for (int j = 0; j < num_resources; j++) {
            needs[i][j] = max_resources[i][j] - allocations[i][j];
        }
    }

    for (int i = 0; i < num_processes; i++) {
        set_process_max(&p[i], max_resources[i]);
        set_process_allocation(&p[i], allocations[i]);
        set_process_need(&p[i], needs[i]);
    }

    // Set up Banker's Task
    BankersTask task;
    init_bankers_task(&task, num_resources, num_processes);
    set_task_processes(&task, p);
    int available_resources[4] = {3, 3,2,2};
    set_task_available(&task, available_resources);

    // Resource requests
    // 进程 0 请求（安全）
int request0[4] = {1, 0, 2, 1};
resource_request(&task, 0, request0);


int request1[4] = {0, 1, 1, 1};
resource_request(&task, 1, request1);

int request2[4] = {1, 0, 2, 0};
resource_request(&task, 2, request2);

int request3[4] = {3, 2, 2, 1};
resource_request(&task, 3, request3);

int request4[4] = {1, 2, 1, 1};
resource_request(&task, 4, request4);

    // Cleanup
    destroy_bankers_task(&task);
    for (int i = 0; i < num_processes; i++) {
        destroy_process(&p[i]);
    }

    return 0;
}
