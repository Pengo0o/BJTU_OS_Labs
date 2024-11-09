#include "/opt/data/private/hyp/BJTU_OS_Labs/LAB10/head/bank.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Process functions
void init_process(Process *process, int num_res) {
    process->num_res = num_res;
    process->max = (int *)malloc(num_res * sizeof(int));
    process->allocation = (int *)malloc(num_res * sizeof(int));
    process->need = (int *)malloc(num_res * sizeof(int));
}

void destroy_process(Process *process) {
    free(process->max);
    free(process->allocation);
    free(process->need);
    process->num_res = 0;
}

void set_process_max(Process *process, const int *max_resources) {
    memcpy(process->max, max_resources, process->num_res * sizeof(int));
}

void set_process_allocation(Process *process, const int *allocated_resources) {
    memcpy(process->allocation, allocated_resources, process->num_res * sizeof(int));
}

void set_process_need(Process *process, const int *needed_resources) {
    memcpy(process->need, needed_resources, process->num_res * sizeof(int));
}

// BankersTask functions
void init_bankers_task(BankersTask *task, int num_res, int num_processes) {
    task->num_res = num_res;
    task->num_process = num_processes;
    task->process = (Process *)malloc(num_processes * sizeof(Process));
    task->available = (int *)malloc(num_res * sizeof(int));
    task->work = (int *)malloc(num_res * sizeof(int));
    task->finish = (int *)calloc(num_processes, sizeof(int));
    task->safe_list = (int *)malloc(num_processes * sizeof(int));
}

void destroy_bankers_task(BankersTask *task) {
    free(task->process);
    free(task->available);
    free(task->work);
    free(task->finish);
    free(task->safe_list);
    task->num_res = 0;
    task->num_process = 0;
}

void set_task_processes(BankersTask *task, Process *processes) {
    memcpy(task->process, processes, task->num_process * sizeof(Process));
}

void set_task_available(BankersTask *task, const int *available_resources) {
    memcpy(task->available, available_resources, task->num_res * sizeof(int));
}

void resource_request(BankersTask *task, int process_id, const int *request) {
    printf(BOLD YELLOW "\nProcessing resource request from Process %d: " RESET, process_id);
    for (int i = 0; i < task->num_res; i++) {
        printf("%d ", request[i]);
    }
    printf("\n");

    Process *proc = &task->process[process_id];

    // Check if request <= need
    for (int i = 0; i < task->num_res; i++) {
        if (request[i] > proc->need[i]) {
            printf(RED "Request denied: Request[%d] (%d) > Need[%d] (%d)\n" RESET,
                   i, request[i], i, proc->need[i]);
            return;
        }
    }

    // Check if request <= available
    for (int i = 0; i < task->num_res; i++) {
        if (request[i] > task->available[i]) {
            printf(RED "Request denied: Request[%d] (%d) > Available[%d] (%d)\n" RESET,
                   i, request[i], i, task->available[i]);
            return;
        }
    }

    // Try to allocate resources
    for (int i = 0; i < task->num_res; i++) {
        task->available[i] -= request[i];
        proc->allocation[i] += request[i];
        proc->need[i] -= request[i];
    }

    // Safety check
    if (safety_check(task)) {
        printf(GREEN "Resource allocated successfully. Safe sequence: " RESET);
        for (int i = 0; i < task->num_process; i++) {
            printf("%d ", task->safe_list[i]);
        }
        printf("\n");
    } else {
        // Roll back allocation
        for (int i = 0; i < task->num_res; i++) {
            task->available[i] += request[i];
            proc->allocation[i] -= request[i];
            proc->need[i] += request[i];
        }
        printf(RED "Resource allocation failed. System is not in a safe state.\n" RESET);
    }
}

int safety_check(BankersTask *task) {
    memcpy(task->work, task->available, task->num_res * sizeof(int));
    memset(task->finish, 0, task->num_process * sizeof(int));
    int count = 0;

    printf(BLUE "Performing safety check...\n" RESET);

    while (count < task->num_process) {
        int found = -1;
        for (int i = 0; i < task->num_process; i++) {
            if (!task->finish[i]) {
                int j;
                for (j = 0; j < task->num_res; j++) {
                    if (task->process[i].need[j] > task->work[j]) {
                        break;
                    }
                }
                if (j == task->num_res) {
                    found = i;
                    break;
                }
            }
        }

        if (found == -1) {
            printf(RED "No suitable process found. System is not in a safe state.\n" RESET);
            return 0;
        }

        printf(CYAN "Process %d can be allocated resources.\n" RESET, found);
        for (int i = 0; i < task->num_res; i++) {
            task->work[i] += task->process[found].allocation[i];
        }
        task->finish[found] = 1;
        task->safe_list[count++] = found;
    }

    printf(GREEN "System is in a safe state.\n" RESET);
    return 1;
}
