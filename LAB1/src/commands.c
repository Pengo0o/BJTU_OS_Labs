#include "../head/color.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "../head/commands.h"

char BUILDIN[][MAX_COMMAND_LENGTH] = {
    "cat",
    "cp",
    "type",
    "cd",
    "exit",
    "ls",
    "pwd"
};

void shell() 
{
    printf(BOLD COLOR_CYAN);
    printf(".______    _______ .__   __.   _______  \n");
    printf("|   _  \\  |   ____||  \\ |  |  /  ____|  \n");
    printf("|  |_)  | |  |__   |   \\|  | |  |  __    \n");
    printf("|   ___/  |   __|  |  . `  | |  | |_ |   \n");
    printf("|  |      |  |____ |  |\\   | |  |__| |   \n");
    printf("| _|      |_______||__| \\__|  \\______|   \n");
    printf(COLOR_RESET);
}

int cmp_strings(const void *A, const void *B) {
    char **a = (char **)A, **b = (char **)B;
    return strcmp(*a, *b);
}

char **list_dir(const char *path) {
    int num_files = 0, max_num_files = 16;
    char **file_names = malloc((max_num_files + 1) * sizeof(char *));
    if (!file_names) { perror("malloc"); return NULL; }
    DIR *dir = opendir(path);
    if (!dir) { perror("opendir"); free(file_names); return NULL; }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (num_files == max_num_files) {
            max_num_files *= 2;
            char **temp = realloc(file_names, (max_num_files + 1) * sizeof(char *));
            if (!temp) { perror("realloc"); break; }
            file_names = temp;
        }
        file_names[num_files] = strdup(entry->d_name);
        if (!file_names[num_files]) { perror("strdup"); break; }
        num_files++;
    }
    file_names[num_files] = NULL;
    closedir(dir);
    return file_names;
}

void free_file_names(char **file_names) {
    if (file_names)
        for (int i = 0; file_names[i] != NULL; i++)
            free(file_names[i]);
    free(file_names);
}



int print_file_names(const char *path, int show_hidden, int print_path) {
    struct winsize size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
    int cols = size.ws_col, current = 0;
    char **file_names = list_dir(path);
    if (!file_names) return 0;
    if (print_path) printf("%s:\n", path);
    int num_files = 0;
    while (file_names[num_files] != NULL) num_files++;
    qsort(file_names, num_files, sizeof(char *), cmp_strings);
    for (int i = 0; i < num_files; i++) {
        if (file_names[i][0] != '.' || show_hidden) {
            int name_length = strlen(file_names[i]);
            if (current + name_length + 2 > cols) {
                printf("\n");
                current = 0;
            }
            if (current > 0) {
                printf("  ");
                current += 2;
            }
            print_colored_name(path, file_names[i]);
            current += name_length;
        }
    }
    printf("\n");
    free_file_names(file_names);
    return 1;
}

int is_dir(const char *path) {
    struct stat statbuf;
    if (stat(path, &statbuf) != 0) return 0;
    return S_ISDIR(statbuf.st_mode);
}

void split_path(const char *file, char *dir, char *file_name) {
    if (!file) return;
    const char *last_slash = strrchr(file, '/');
    if (last_slash) {
        if (dir) {
            size_t dir_len = last_slash - file;
            strncpy(dir, file, dir_len);
            dir[dir_len] = '\0';
        }
        if (file_name) strcpy(file_name, last_slash + 1);
    } else {
        if (dir) strcpy(dir, ".");
        if (file_name) strcpy(file_name, file);
    }
}

void copy_file(const char *src_path, const char *dst_path) {
    int src_fd, dst_fd, cnt;
    char BUFFER[2048];
    struct stat stat_buf;
    if (stat(src_path, &stat_buf) != 0) { perror("stat"); return; }
    mode_t mode = stat_buf.st_mode;
    src_fd = open(src_path, O_RDONLY);
    if (src_fd < 0) { perror("open src"); return; }
    dst_fd = open(dst_path, O_WRONLY | O_CREAT | O_TRUNC, mode);
    if (dst_fd < 0) { perror("open dst"); close(src_fd); return; }
    while ((cnt = read(src_fd, BUFFER, sizeof(BUFFER))) > 0)
        if (write(dst_fd, BUFFER, cnt) != cnt) { perror("write"); break; }
    if (cnt < 0) perror("read");
    close(src_fd);
    close(dst_fd);
}

void get_external_path(const char *command, char *command_path) {
    if (!command_path) return;
    char *path_env = getenv("PATH");
    if (!path_env) { strcpy(command_path, "#"); return; }
    char *path_dup = strdup(path_env);
    char *token = strtok(path_dup, ":");
    int found = 0;
    while (token) {
        char temp[256];
        snprintf(temp, sizeof(temp), "%s/%s", token, command);
        if (access(temp, X_OK) == 0) {
            strcpy(command_path, temp);
            found = 1;
            break;
        }
        token = strtok(NULL, ":");
    }
    if (!found) strcpy(command_path, "#");
    free(path_dup);
}

void ls(const char commands[MAX_COMMAND_ARGC][MAX_COMMAND_LENGTH], int commands_length) {
    int show_hidden = 0;
    char path[MAX_COMMAND_LENGTH] = "./";
    if (commands_length > 1)
        for (int i = 1; i < commands_length; i++) {
            if (strcmp(commands[i], "-a") == 0)
                show_hidden = 1;
            else
                strcpy(path, commands[i]);
        }
    if (!print_file_names(path, show_hidden, 0))
        printf(COLOR_RED "Error: %s does not exist!\n" COLOR_RESET, path);
}

void pwd(const char commands[MAX_COMMAND_ARGC][MAX_COMMAND_LENGTH], int commands_length) {
    if (commands_length != 1) printf(COLOR_RED "Error: too many parameters!\n" COLOR_RESET);
    else {
        char cwd[256];
        if (getcwd(cwd, sizeof(cwd))) printf("%s\n", cwd);
        else perror("getcwd");
    }
}

void cat(const char commands[MAX_COMMAND_ARGC][MAX_COMMAND_LENGTH], int commands_length) {
    if (commands_length < 2) printf(COLOR_RED "Error: at least one parameter needed\n" COLOR_RESET);
    else {
        char BUFFER[256];
        int cnt;
        for (int i = 1; i < commands_length; i++) {
            int fd = open(commands[i], O_RDONLY);
            if (fd == -1) { perror("open"); continue; }
            while ((cnt = read(fd, BUFFER, sizeof(BUFFER))) > 0)
                write(STDOUT_FILENO, BUFFER, cnt);
            if (cnt < 0) perror("read");
            close(fd);
        }
    }
}

void cp(const char commands[MAX_COMMAND_ARGC][MAX_COMMAND_LENGTH], int commands_length) {
    if (commands_length != 3) printf(COLOR_RED "Error: cp requires two parameters\n" COLOR_RESET);
    else {
        if (access(commands[1], F_OK) == -1)
            printf(COLOR_RED "Error: %s does not exist!\n" COLOR_RESET, commands[1]);
        else {
            if (is_dir(commands[2])) {
                char file_name[MAX_COMMAND_LENGTH];
                split_path(commands[1], NULL, file_name);
                char dst[MAX_COMMAND_LENGTH * 2];
                if (commands[2][strlen(commands[2]) - 1] == '/')
                    snprintf(dst, sizeof(dst), "%s%s", commands[2], file_name);
                else
                    snprintf(dst, sizeof(dst), "%s/%s", commands[2], file_name);
                copy_file(commands[1], dst);
            } else {
                copy_file(commands[1], commands[2]);
            }
        }
    }
}

void exitshell(const char commands[MAX_COMMAND_ARGC][MAX_COMMAND_LENGTH], int commands_length) {
    if (commands_length == 1) exit(0);
    else if (commands_length == 2) {
        int exit_code = atoi(commands[1]);
        exit(exit_code);
    } else printf(COLOR_RED "Error: too many parameters!\n" COLOR_RESET);
}

void cd(const char commands[MAX_COMMAND_ARGC][MAX_COMMAND_LENGTH], int commands_length) {
    const char *path = NULL;
    if (commands_length == 1) {
        path = getenv("HOME");
        if (!path) path = "/";
    } else if (commands_length == 2)
        path = commands[1];
    else { printf(COLOR_RED "Error: too many parameters!\n" COLOR_RESET); return; }
    if (chdir(path) != 0) perror("cd");
}

void type(const char commands[MAX_COMMAND_ARGC][MAX_COMMAND_LENGTH], int commands_length) {
    if (commands_length != 2) printf(COLOR_RED "Error: need exactly one parameter!\n" COLOR_RESET);
    else {
        int is_builtin = 0;
        for (int i = 0; i < BUILDIN_NUM; i++)
            if (strcmp(commands[1], BUILDIN[i]) == 0) {
                is_builtin = 1;
                printf("%s is a built-in shell command.\n", commands[1]);
                break;
            }
        if (!is_builtin) {
            char external_path[256];
            get_external_path(commands[1], external_path);
            if (external_path[0] != '#')
                printf("%s is an external command located at %s\n", commands[1], external_path);
            else
                printf(COLOR_RED "Error: %s does not exist!\n" COLOR_RESET, commands[1]);
        }
    }
}
void env(const char commands[MAX_COMMAND_ARGC][MAX_COMMAND_LENGTH],int command_length){
    char* env_param;
    if (strcmp(commands[0],"getenv") == 0)
    {
        if (command_length>2) printf(COLOR_RED"Error: too much parameters!\n" COLOR_RESET);
        else if (command_length == 2){
            env_param = getenv(commands[1]);
            if (!env_param) printf(COLOR_RED"Error: %s is not existed!\n"COLOR_RESET,commands[1]);
            else printf("%s:%s\n",commands[1],env_param);
        }
        else printf(COLOR_RED"Error: %s need 1 parameters!\n"COLOR_RESET,commands[0]);
    }
    else if (strcmp(commands[0],"setenv")==0)
    {
        if (command_length>4) printf(COLOR_RED"Error: too much parameters!\n" COLOR_RESET);
        else if (command_length == 4){
            if (setenv(commands[1],commands[2],atoi(commands[3]))==0) printf("Successful!\n");
            else printf(COLOR_RED"Failed!\n"COLOR_RESET); 
        }
        else printf(COLOR_RED "Error: %s need 3 parameters!\n"COLOR_RESET,commands[0]);

    }
    else{
        if (command_length>2) printf(COLOR_RED"Error: too much parameters!\n"COLOR_RESET);
        else if (command_length == 2){
            if (unsetenv(commands[1])==0) printf("Successful!\n");
            else printf(COLOR_RED"Failed!\n"COLOR_RESET);
        }  
        else printf(COLOR_RED "Error: %s need 1 parameters!\n"COLOR_RESET,commands[0]);
    }
    
}

int parse_input(char *input_line, char commands[MAX_COMMAND_ARGC][MAX_COMMAND_LENGTH]) {
    int argc = 0;
    char *token = strtok(input_line, " \t\n");
    while (token && argc < MAX_COMMAND_ARGC) {
        strncpy(commands[argc], token, MAX_COMMAND_LENGTH - 1);
        commands[argc][MAX_COMMAND_LENGTH - 1] = '\0';
        argc++;
        token = strtok(NULL, " \t\n");
    }
    return argc;
}

void execute_external_command(char commands[MAX_COMMAND_ARGC][MAX_COMMAND_LENGTH], int commands_length) {
    pid_t pid = fork();
    if (pid == 0) {
        char *args[MAX_COMMAND_ARGC + 1];
        for (int i = 0; i < commands_length; i++)
            args[i] = commands[i];
        args[commands_length] = NULL;
        execvp(args[0], args);
        perror("execvp");
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("fork");
    } else
        wait(NULL);
}