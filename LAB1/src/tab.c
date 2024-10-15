#include "../head/commands.h"
#include "../head/tab.h"
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


char *command_generator(const char *text, int state) {
    static int list_index, len;
    char *name;

    if (!state) {
        list_index = 0;
        len = strlen(text);
    }

    while (list_index < BUILDIN_NUM) {
        name = BUILDIN[list_index];
        list_index++;
        if (strncmp(name, text, len) == 0)
            return strdup(name);
    }
    return NULL;
}

char **command_completion(const char *text, int start, int end) {
    char **matches = NULL;

    if (start == 0)
        matches = rl_completion_matches(text, command_generator);
    else
        matches = NULL;  

    return matches;
}

void initialize_readline() {
    rl_readline_name = "yunpengshell";
    rl_attempted_completion_function = command_completion;
}

