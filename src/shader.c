#include <vulkan/vulkan.h>

#include <stdio.h>
#include <stdlib.h>

#include "custom_type.h"
#include "shader.h"

VkShaderModule create_shader_module(VkDevice device, const char* file_path)
{
        VkShaderModule shader_module;

        S_string file_content = fn_readFile(file_path);
        if(file_content.error != NO_ERROR)
                return VK_NULL_HANDLE;

        VkShaderModuleCreateInfo create_info =
                {
                        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                        .codeSize = file_content.size,
                        .pCode = file_content.p_buffer
                };

        VkResult result = vkCreateShaderModule(device, &create_info, NULL, &shader_module);
        if(result != VK_SUCCESS)
        {
                fprintf(stderr, "Error : shader module creation failed [%d] \"%s\"\n", result, fn_getVulkanError(result));
                return VK_NULL_HANDLE;
        }

        free(file_content.p_buffer);
        return shader_module;
}

