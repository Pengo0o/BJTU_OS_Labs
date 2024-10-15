#ifndef TAB_H_
#define TAB_H_
char *command_generator(const char *text, int state);
char **command_completion(const char *text, int start, int end) ;
void initialize_readline();

#endif