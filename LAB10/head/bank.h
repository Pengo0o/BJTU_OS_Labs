#ifndef BANKERS_H
#define BANKERS_H

// Define color codes for output
#define RESET   "\033[0m"
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define BOLD    "\033[1m"       /* Bold */

typedef struct {
    int num_res;
    int *max;
    int *allocation;
    int *need;
} Process;

typedef struct {
    int num_res;
    int num_process;
    Process *process;
    int *available;
    int *work;
    int *finish;
    int *safe_list;
} BankersTask;

void init_process(Process *process, int num_res);
void destroy_process(Process *process);
void set_process_max(Process *process, const int *max_resources);
void set_process_allocation(Process *process, const int *allocated_resources);
void set_process_need(Process *process, const int *needed_resources);

void init_bankers_task(BankersTask *task, int num_res, int num_processes);
void destroy_bankers_task(BankersTask *task);
void set_task_processes(BankersTask *task, Process *processes);
void set_task_available(BankersTask *task, const int *available_resources);

void resource_request(BankersTask *task, int process_id, const int *request);
int safety_check(BankersTask *task);



#endif // BANKERS_H
