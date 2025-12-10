#ifndef GLOBAL_VARIABLE_H
#define GLOBAL_VARIABLE_H

extern int global_argc;
extern char** global_argv;

#ifdef DEBUG
extern int validation_layer_count;
extern const char* validation_layer [];
#endif

void manage_main_arg(int argc, char** argv);

#endif
