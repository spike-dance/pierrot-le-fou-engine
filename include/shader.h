#ifndef SHADER_H
#define SHADER_H

#include <my_std.h>
#include <vulkan/vulkan.h>

typedef
{
        E_error error;
        VkBuffer buffer;
        VkDeviceMemory deviceMemory;
} S_vertexBuffer;

VkShaderModule create_shader_module(VkDevice device, const char* file_path);

VkBuffer fn_createVertexBuffer();

#endif
