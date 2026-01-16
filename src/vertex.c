#include <vulkan/vulkan.h>
#include <cglm/cglm.h>

#include <stdio.h>

#include <shader.h>
#include "error.h"

S_vertexBuffer fn_createVertexBuffer(VkPhysicalDevice physicalDevice, VkDevice device)
{
        S_vertexBuffer vertexBuffer = {0};

        VkBufferCreateInfo createInfo =
                {
                        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                        .size = ,
                        .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
                };

        VkResult result = vkCreateBuffer(device, &createInfo, NULL, &vertexBuffer.buffer);
        if(result != VK_SUCCESS)
        {
                fprintf(stderr, "Error : vertex buffer creation failed [%d] \"%s\"\n", result, fn_getVulkanError(result));
                goto GO_END_ERROR;
        }

        VkMemoryRequirements memRequirement;
        vkGetBufferMemoryRequirements(device, vertexBuffer.buffer, &memRequirement);

        VkPhysicalDeviceMemoryProperties memProperty;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperty);


        u32 memIndex = 0;
        for(u32 i=0; i<memProperty.memoryTypeCount; i++)
        {
                if(memRequirement.memoryTypeBits & (1 << i) && (memProperty.memoryTypes[i].propertyFlags & (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
                {
                        memIndex = i;
                        break;
                }
        }

        VkMemoryAllocateInfo allocInfo =
                {
                        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                        .allocationSize = memRequirement.size,
                        .memoryTypeIndex = memIndex
                };

        result = vkAllocateMemory(device, &allocInfo, NULL, &vertexBuffer.deviceMemory);
        if(result != VK_SUCCESS)
        {
                fprintf(stderr, "Error : vertex buffer allocation failed [%d] \"%s\"\n", result, fn_getVulkanError(result));
                goto GO_END_ERROR;
        }

        vkBindBufferMemory(device, vertexBuffer.buffer, vertexBuffer.deviceMemory, 0);

        return vertexBuffer;
//_____________________________________________







GO_END_ERROR:
        vertexBuffer.error = CREATION_FAILED;
        return vertexBuffer;
}
