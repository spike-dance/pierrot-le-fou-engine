#include <stdio.h>

#include "error.h"
#include "global_variable.h"

int global_argc = 0;
char** global_argv = NULL;


#ifdef DEBUG
int debug_extension_count = 1;
int validation_layer_count = 1;
const char* debug_extension_name [] = { VK_EXT_DEBUG_UTILS_EXTENSION_NAME };
const char* validation_layer [] = {"VK_LAYER_KHRONOS_validation"};
#else
int debug_extension_count = 0;
#endif

void manage_main_arg(int argc, char** argv)
{
        global_argc = argc;
        global_argv = argv;
}
