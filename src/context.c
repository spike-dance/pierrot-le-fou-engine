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

        context.swapchain = create_swapchain(context.physical_device, context.device, context.surface, &context.swapchain_format);
        if(context.swapchain == VK_NULL_HANDLE)
                goto ERROR;

        vkGetSwapchainImagesKHR(context.device, context.swapchain, &context.image_count, NULL);
        context.swapchain_image = malloc(sizeof(VkImage) * context.image_count);
        vkGetSwapchainImagesKHR(context.device, context.swapchain, &context.image_count, context.swapchain_image);

        context.swapchain_image_view = get_swapchain_image_view(context.device, context.swapchain, context.swapchain_format, context.swapchain_image, context.image_count);
        if(context.swapchain_image_view == NULL)
                goto ERROR;

        printf("context creation success\n");

        return context; //END
//__________________________________________________


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

        VkResult result = vkCreateDevice(physical_device, &create_info, NULL, &device);
        if(result != VK_SUCCESS)
        {
                fprintf(stderr, "Error : physical device creation failed [%d] \"%s\"\n", result, get_vulkan_error(result));
                return VK_NULL_HANDLE;
        }

        return device;
}

VkSwapchainKHR create_swapchain(VkPhysicalDevice physical_device, VkDevice device, VkSurfaceKHR surface, VkFormat* swapchain_format)
{
        VkSwapchainKHR swapchain;

        VkSurfaceCapabilitiesKHR capability;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &capability);

        VkSurfaceFormatKHR* format_vec;
        VkSurfaceFormatKHR format;

        int format_count;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, NULL);
        format_vec = malloc(sizeof(VkSurfaceFormatKHR) * format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, format_vec);

        bool format_found = false;
        for(int i=0; i<format_count; i++)
        {
                if(format_vec[i].format == VK_FORMAT_B8G8R8A8_SRGB &&
                   format_vec[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                {
                        printf("format found\n");
                        format = format_vec[i];
                        format_found = true;
                        break;
                }
        }

        if(!format_found)
        {
                format = format_vec[0];
                printf("test\n");
        }

        free(format_vec);

        VkPresentModeKHR* present_mode_vec;
        VkPresentModeKHR present_mode;

        int present_mode_count;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, NULL);
        present_mode_vec = malloc(sizeof(VkPresentModeKHR) * present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, present_mode_vec);

        for(int i=0; i<present_mode_count; i++)
        {
                if(present_mode_vec[i] == VK_PRESENT_MODE_MAILBOX_KHR)
                {
                        present_mode = VK_PRESENT_MODE_MAILBOX_KHR;
                        break;
                }
        }

        free(present_mode_vec);




        VkSwapchainCreateInfoKHR create_info =
                {
                        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                        .surface = surface,

                        .minImageCount = capability.minImageCount + 1,
                        .imageFormat = format.format,
                        .imageColorSpace = format.colorSpace,
                        .imageExtent = capability.currentExtent,
                        .imageArrayLayers = 1,
                        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,

                        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,

                        .preTransform = capability.currentTransform,

                        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,

                        .presentMode = present_mode,
                        .clipped = VK_TRUE,
                        .oldSwapchain = VK_NULL_HANDLE,
                };





        VkResult result = vkCreateSwapchainKHR(device, &create_info, NULL, &swapchain);
        if(result != VK_SUCCESS)
        {
                fprintf(stderr, "Error : swap chain creation failed [%d] \"%s\"\n", result, get_vulkan_error(result));
                return VK_NULL_HANDLE;
        }
        *swapchain_format = format.format;
        return swapchain;
}

VkImageView* get_swapchain_image_view(VkDevice device, VkSwapchainKHR swapchain, VkFormat swapchain_format, VkImage* swapchain_image, int image_count)
{
        VkImageView* swapchain_image_view = malloc(sizeof(VkImageView) * image_count);

        VkImageViewCreateInfo create_info =
                {
                        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                        .viewType = VK_IMAGE_VIEW_TYPE_2D,
                        .format = swapchain_format,
                        .components =
                        {
                                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                                .a = VK_COMPONENT_SWIZZLE_IDENTITY
                        },
                        .subresourceRange =
                        {
                                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                                .baseMipLevel = 0,
                                .levelCount = 1,
                                .baseArrayLayer = 0,
                                .layerCount = 1,
                        }
                };

        for(int i=0; i<image_count; i++)
        {
                create_info.image = swapchain_image[i];
                VkResult result = vkCreateImageView(device, &create_info, NULL, &swapchain_image_view[i]);
                if(result != VK_SUCCESS)
                {
                        fprintf(stderr, "Error : image view %d creation failed [%d] \"%s\"\n", i, result, get_vulkan_error(result));
                        return NULL;
                }
        }

        return swapchain_image_view;
}

void clear_context(Vulkan_context context)
{
        for(int i=0; i<context.image_count; i++)
        {
                vkDestroyImageView(context.device, context.swapchain_image_view[i], NULL);
        }

        free(context.swapchain_image);
        free(context.swapchain_image_view);

        vkDestroySwapchainKHR(context.device, context.swapchain, NULL);
        vkDestroySurfaceKHR(context.instance, context.surface, NULL);
        vkDestroyDevice(context.device, NULL);
        vkDestroyInstance(context.instance, NULL);
}
