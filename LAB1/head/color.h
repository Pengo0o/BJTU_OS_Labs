#ifndef COLOR_H_
#define COLOR_H_

// 定义 ANSI 颜色代码
#define COLOR_RESET   "\033[0m"
#define BOLD          "\033[1m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_WHITE   "\033[37m"

// 定义文件类型颜色
#define COLOR_DIR     COLOR_BLUE
#define COLOR_FILE    COLOR_WHITE
#define COLOR_EXEC    COLOR_GREEN

void print_colored_name(const char *path, const char *name);
void print_prompt();

#endif