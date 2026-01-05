#ifndef ERROR_H
#define ERROR_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

const char* fn_getVulkanError(VkResult result);

void fn_glfwErrorCallback(int errorCode, const char* v_errorString);

#endif
