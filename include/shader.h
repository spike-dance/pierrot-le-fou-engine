#ifndef SHADER_H
#define SHADER_H

#include <vulkan/vulkan.h>

VkShaderModule create_shader_module(VkDevice device, char* file_path);

#endif
