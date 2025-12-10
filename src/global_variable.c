#include <stdio.h>

#include "error.h"
#include "global_variable.h"

int global_argc = 0;
char** global_argv = NULL;

#ifdef DEBUG
int validation_layer_count = 1;
const char* validation_layer [] = {"VK_LAYER_KHRONOS_validation"};
#endif

void manage_main_arg(int argc, char** argv)
{
        global_argc = argc;
        global_argv = argv;
}
