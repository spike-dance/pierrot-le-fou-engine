#ifndef GLOBAL_VARIABLE_H
#define GLOBAL_VARIABLE_H

#include <my_std.h>

extern i32 g_argc;
extern char** gvv_argv;

extern const char* gcvv_deviceExtension [];
extern cu32 gc_deviceExtensionCount;

extern cu32 gc_debugExtensionCount;

#ifdef DEBUG
extern const char* gcvv_debugExtensionName [];
extern const char* gcvv_validationLayer [];
extern cu32 gc_validationLayerCount;
#endif

void fn_manageMainArg(i32 argc, char** argv);

#endif
