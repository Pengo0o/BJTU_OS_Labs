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
#include "../head/color.h"

void print_prompt() {
    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd");
        strcpy(cwd, "?");
    }

    char *username = getenv("USER");
    if (!username) username = "unknown";
    
    printf(BOLD COLOR_GREEN "[%s@" COLOR_RESET, username);
    printf(BOLD COLOR_BLUE "%s",cwd);
    printf(BOLD COLOR_GREEN "]$ "COLOR_RESET);
    fflush(stdout);
}

void print_colored_name(const char *path, const char *name) {
    struct stat statbuf;
    char full_path[MAX_COMMAND_LENGTH * 2];
    if (path[strlen(path) - 1] == '/')
        snprintf(full_path, sizeof(full_path), "%s%s", path, name);
    else
        snprintf(full_path, sizeof(full_path), "%s/%s", path, name);

    if (lstat(full_path, &statbuf) == -1) {
        perror("lstat");
        printf("%s", name);
        return;
    }
    if (S_ISDIR(statbuf.st_mode))
        printf(COLOR_DIR "%s" COLOR_RESET, name);
    else if (statbuf.st_mode & S_IXUSR)
        printf(COLOR_EXEC "%s" COLOR_RESET, name);
    else
        printf(COLOR_FILE "%s" COLOR_RESET, name);
}