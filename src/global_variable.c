#include <my_std.h>
#include <stdio.h>

#include "error.h"
#include "global_variable.h"

i32 g_argc = 0;
char** gvv_argv = NULL;

const char* gcvv_deviceExtension [] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME};
cu32 gc_deviceExtensionCount = sizeof(gcvv_deviceExtension)/sizeof(char*);

#ifdef DEBUG
const char* gcvv_debugExtensionName [] = { VK_EXT_DEBUG_UTILS_EXTENSION_NAME };
const char* gcvv_validationLayer [] = {"VK_LAYER_KHRONOS_validation"};
cu32 gc_validationLayerCount = 1;
cu32 gc_debugExtensionCount = 1;
#else
cu32 gc_debugExtensionCount = 0;
#endif

void fn_manageMainArg(i32 argc, char** argv)
{
        g_argc = argc;
        gvv_argv = argv;
}
