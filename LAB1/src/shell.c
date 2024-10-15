#include "../head/color.h"
#include "../head/shell.h"
#include "../head/commands.h"
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
#include <readline/readline.h>
#include <readline/history.h>

int _main() {
    shell();
    char input_line[1024];
    char commands[MAX_COMMAND_ARGC][MAX_COMMAND_LENGTH];
    int commands_length;
    while (1) {
        print_prompt();

        char *input = readline("");  
        if (!input) break; 
        if (*input) add_history(input);
        strncpy(input_line, input, sizeof(input_line) - 1);
        input_line[sizeof(input_line) - 1] = '\0';
        free(input);

        commands_length = parse_input(input_line, commands);
        if (commands_length == 0) continue;
        if (strcmp(commands[0], "exit") == 0)
            exitshell(commands, commands_length);
        else if (strcmp(commands[0], "cd") == 0)
            cd(commands, commands_length);
        else if (strcmp(commands[0], "ls") == 0)
            ls(commands, commands_length);
        else if (strcmp(commands[0], "pwd") == 0)
            pwd(commands, commands_length);
        else if (strcmp(commands[0], "cat") == 0)
            cat(commands, commands_length);
        else if (strcmp(commands[0], "cp") == 0)
            cp(commands, commands_length);
        else if (strcmp(commands[0], "type") == 0)
            type(commands, commands_length);
        else if (strstr(commands[0],"env") !=NULL)
            env(commands,commands_length);
        else
            execute_external_command(commands, commands_length);
    }
    return 0;
}
