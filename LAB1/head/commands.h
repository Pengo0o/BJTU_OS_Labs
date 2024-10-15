#ifndef COMMANDS_H_
#define COMMANDS_H_

#define MAX_COMMAND_LENGTH 256
#define MAX_COMMAND_ARGC 10
#define BUILDIN_NUM 7
extern char BUILDIN[][MAX_COMMAND_LENGTH];
void shell() ;
int cmp_strings(const void *A, const void *B);
char **list_dir(const char *path);
void free_file_names(char **file_names);
int print_file_names(const char *path, int show_hidden, int print_path);
int is_dir(const char *path);
void split_path(const char *file, char *dir, char *file_name);
void copy_file(const char *src_path, const char *dst_path) ;
void get_external_path(const char *command, char *command_path);
void ls(const char commands[MAX_COMMAND_ARGC][MAX_COMMAND_LENGTH], int commands_length);
void pwd(const char commands[MAX_COMMAND_ARGC][MAX_COMMAND_LENGTH], int commands_length);
void cat(const char commands[MAX_COMMAND_ARGC][MAX_COMMAND_LENGTH], int commands_length);
void cp(const char commands[MAX_COMMAND_ARGC][MAX_COMMAND_LENGTH], int commands_length);
void exitshell(const char commands[MAX_COMMAND_ARGC][MAX_COMMAND_LENGTH], int commands_length) ;
void cd(const char commands[MAX_COMMAND_ARGC][MAX_COMMAND_LENGTH], int commands_length);
void type(const char commands[MAX_COMMAND_ARGC][MAX_COMMAND_LENGTH], int commands_length);
void env(const char commands[MAX_COMMAND_ARGC][MAX_COMMAND_LENGTH],int command_length);
int parse_input(char *input_line, char commands[MAX_COMMAND_ARGC][MAX_COMMAND_LENGTH]) ;
void execute_external_command(char commands[MAX_COMMAND_ARGC][MAX_COMMAND_LENGTH], int commands_length) ;
#endif