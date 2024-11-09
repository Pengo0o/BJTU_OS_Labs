#include "/home/peng/BJTU_OS_Labs/LAB2/head/bank.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    printf(BOL+D BLUE "22331170-YunpengHuan-banks\n" RESET);

    const int num_processes = 5;
    const int num_resources = 3;

    Process p[num_processes];
    for (int i = 0; i < num_processes; i++) {
        init_process(&p[i], num_resources);
    }

    // Set up processes
    int max_resources[5][3] = {
        {7, 5, 3},
        {3, 2, 2},
        {9, 0, 2},
        {2, 2, 2},
        {4, 3, 3}
    };
    int allocations[5][3] = {
        {0, 1, 0},
        {2, 0, 0},
        {3, 0, 2},
        {2, 1, 1},
        {0, 0, 2}
    };
    int needs[5][3] = {
        {7, 4, 3},
        {1, 2, 2},
        {6, 0, 0},
        {0, 1, 1},
        {4, 3, 1}
    };

    for (int i = 0; i < num_processes; i++) {
        set_process_max(&p[i], max_resources[i]);
        set_process_allocation(&p[i], allocations[i]);
        set_process_need(&p[i], needs[i]);
    }

    // Set up Banker's Task
    BankersTask task;
    init_bankers_task(&task, num_resources, num_processes);
    set_task_processes(&task, p);
    int available_resources[3] = {3, 3, 2};
    set_task_available(&task, available_resources);

    // Resource requests
    int request0[3] = {0, 0, 0};
    resource_request(&task, 0, request0); // T0

    int request1[3] = {1, 0, 2};
    resource_request(&task, 1, request1); // T1

    int request2[3] = {3, 3, 0};
    resource_request(&task, 4, request2); // T2

    int request3[3] = {0, 2, 0};
    resource_request(&task, 0, request3); // T3

    int request4[3] = {0, 1, 0};
    resource_request(&task, 0, request4); // T4

    // Cleanup
    destroy_bankers_task(&task);
    for (int i = 0; i < num_processes; i++) {
        destroy_process(&p[i]);
    }

    return 0;
}
