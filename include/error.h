#ifndef ERROR_H
#define ERROR_H

#include <vulkan/vulkan.h>

typedef enum
{
        NO_ERROR = 0,
        CREATION_FAILED,
} Error_enum;

const char* get_vulkan_error(VkResult result);

#endif
