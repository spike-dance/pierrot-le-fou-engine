#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "context.h"
#include "error.h"
#include "global_variable.h"

Vulkan_context create_context(GLFWwindow* window)
{
        Vulkan_context context = {.swap_extent = {800,500}};

        context.instance = create_instance();
        if(context.instance == VK_NULL_HANDLE)
                goto ERROR;

        context.physical_device = find_physical_device(context.instance);
        if(context.physical_device == VK_NULL_HANDLE)
                goto ERROR;

        context.queue_family_indice = find_queue_family(context.physical_device);
        if(context.queue_family_indice.error == CREATION_FAILED)
                goto ERROR;

        context.device = create_device(context.physical_device, context.queue_family_indice);
        if(context.device == VK_NULL_HANDLE)
                goto ERROR;

        glfwCreateWindowSurface(context.instance, window, NULL, &context.surface);

        context.swapchain = create_swapchain(context.physical_device, context.device, context.surface, &context.swap_extent);

        return context; //END
//_________________________________________________


ERROR:
        context.error = CREATION_FAILED;
        return context;
}

VkInstance create_instance()
{
        VkInstance instance;
        VkApplicationInfo app_info =
                {
                        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                        .pApplicationName = "Scene",
                        .applicationVersion = VK_MAKE_VERSION(1,0,0),
                        .pEngineName = "bebop engine",
                        .engineVersion = VK_MAKE_VERSION(1,0,0),
                        .apiVersion = VK_API_VERSION_1_0
                };

        int extention_count = 0;
        const char** glfw_extension = glfwGetRequiredInstanceExtensions(&extention_count);
        const char** extention = malloc(sizeof(char*) * (extention_count+1));

        for(int i=0; i<extention_count; i++)
        {
                printf("extention glfw %s\n", glfw_extension[i]);
                extention[i] = glfw_extension[i];
        }

        extention[extention_count] = "VK_EXT_debug_utils";


        int layer_count;
        vkEnumerateInstanceLayerProperties(&layer_count, NULL);
        VkLayerProperties* layer = malloc(sizeof(VkLayerProperties) * layer_count);
        vkEnumerateInstanceLayerProperties(&layer_count, layer);

        for(int i=0; i<layer_count; i++)
        {
                printf("%s  ", layer[i].layerName);
        }
        printf("\n");

        VkInstanceCreateInfo create_info =
                {
                        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                        .pApplicationInfo = &app_info,
                        .enabledExtensionCount = extention_count+1,
                        .ppEnabledExtensionNames = extention,

                        .enabledLayerCount = validation_layer_count,
                        .ppEnabledLayerNames = validation_layer,

                };

        VkResult result = vkCreateInstance(&create_info, NULL, &instance);
        if(result != VK_SUCCESS)
        {
                fprintf(stderr, "Instance creation failed %d \"%s\"\n", result, get_vulkan_error(result));
                return VK_NULL_HANDLE;
        }

        return instance;
}

VkPhysicalDevice find_physical_device(VkInstance instance)
{
        VkPhysicalDevice physical_device;

        int physical_device_count;
        vkEnumeratePhysicalDevices(instance, &physical_device_count, NULL);
        if(physical_device_count == 0)
        {
                fprintf(stderr, "Error : unable to find a compatible gpu with vulkan\n");
                return VK_NULL_HANDLE;
        }

        VkPhysicalDevice* physical_device_vec = malloc(sizeof(VkPhysicalDevice) * physical_device_count);
        vkEnumeratePhysicalDevices(instance, &physical_device_count, physical_device_vec);

        for(int i=0; i<physical_device_count; i++)
        {
                if(is_physical_device_usable(physical_device_vec[i]))
                {
                        physical_device = physical_device_vec[i];
                        break;
                }
        }

        free(physical_device_vec);
        return physical_device;
}
bool is_physical_device_usable(VkPhysicalDevice physical_device)
{
        return true;
}

Queue_family_indice find_queue_family(VkPhysicalDevice physical_device)
{
        Queue_family_indice queue_family_indice =
                {
                        .graphique = -1,
                        .compute = -1,
                        .transfert = -1,
                };

        int queue_family_count;
        vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, NULL);
        VkQueueFamilyProperties* queue_family = malloc(sizeof(VkQueueFamilyProperties) * queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_family);

        for(int i=0; i<queue_family_count; i++)
        {
                if(queue_family[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                        queue_family_indice.graphique = i;

                if(queue_family[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
                        queue_family_indice.compute = i;

                if(queue_family[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
                        queue_family_indice.transfert = i;

                if(queue_family_indice.graphique == i &&
                        queue_family_indice.compute == i &&
                        queue_family_indice.transfert == i)
                {
                        fprintf(stderr, "queue family is shared.\n");
                        break;
                }
        }

        return queue_family_indice;
}

VkDevice create_device(VkPhysicalDevice physical_device, Queue_family_indice queue_family_indice)
{
        VkDevice device;

        float a [] = {1.0f};

        VkDeviceQueueCreateInfo device_queue_info =
                {
                        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                        .queueFamilyIndex = 0, //to change in the future
                        .queueCount = 1,
                        .pQueuePriorities = a,

                };

        VkPhysicalDeviceFeatures device_feature = {0};

        const char* device_extension [] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        VkDeviceCreateInfo create_info =
                {
                        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                        .pQueueCreateInfos = &device_queue_info,
                        .queueCreateInfoCount = 1,
                        .pEnabledFeatures = &device_feature,

                        .enabledExtensionCount = 1,
                        .ppEnabledExtensionNames = device_extension,

                };

        printf("validation_layer = %d\n", validation_layer_count);
        printf("\n\n%s\n\n", validation_layer[0]);

        printf("test\n");
        VkResult result = vkCreateDevice(physical_device, &create_info, NULL, &device);
        printf("test\n");
        if(result != VK_SUCCESS)
        {
                fprintf(stderr, "Error : physical device creation failed [%d] \"%s\"\n", result, get_vulkan_error(result));
                return VK_NULL_HANDLE;
        }

        return device;
}

VkSwapchainKHR create_swapchain(VkPhysicalDevice physical_device, VkDevice device, VkSurfaceKHR surface, VkExtent2D* swap_extent)
{
        VkSwapchainKHR swapchain;

        VkResult result = vkCreateSwapchainKHR(device, NULL, NULL, NULL);
        if(result != VK_SUCCESS)
        {
                fprintf(stderr, "Error : swap chain creation failed [%d] \"%s\"\n", result, get_vulkan_error(result));
                return VK_NULL_HANDLE;
        }
        return swapchain;
}

void clear_context(Vulkan_context context)
{
        vkDestroySwapchainKHR(context.device, context.swapchain, NULL);
        vkDestroySurfaceKHR(context.instance, context.surface, NULL);
        vkDestroyDevice(context.device, NULL);
        vkDestroyInstance(context.instance, NULL);
}
