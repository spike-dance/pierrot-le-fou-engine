#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "context.h"
#include "error.h"
#include "global_variable.h"

S_vulkanContext create_context(GLFWwindow* window)
{
        FILE* file = fopen("/dev/null", "w");
        S_vulkanDebugCallbackUserArg* testtest = malloc(sizeof(S_vulkanDebugCallbackUserArg));
        testtest->verbose_stream = file;
        testtest->info_stream = file;
        testtest->warning_stream = stderr;
        testtest->error_stream = stderr;
        S_vulkanContext context = {.s_swapExtent = {800,500}, .ps_debugCallbackArg = testtest, .maxImageInFlight = 2};

        context.instance = create_instance();
        if(context.instance == VK_NULL_HANDLE)
                goto ERROR;

        context.debugCallback = create_vulkan_debug_callback(context.instance, context.ps_debugCallbackArg);

        context.physicalDevice = find_physical_device(context.instance);
        if(context.physicalDevice == VK_NULL_HANDLE)
                goto ERROR;

        context.s_queueFamilyIndice = find_queue_family(context.physicalDevice);
        if(context.s_queueFamilyIndice.error == CREATION_FAILED)
                goto ERROR;

        context.device = create_device(context.physicalDevice, context.s_queueFamilyIndice);
        if(context.device == VK_NULL_HANDLE)
                goto ERROR;

        glfwCreateWindowSurface(context.instance, window, NULL, &context.surface);

        context.swapchain = create_swapchain(context.physicalDevice, context.device, context.surface, context.s_swapExtent, &context.swapchainFormat);
        if(context.swapchain == VK_NULL_HANDLE)
                goto ERROR;

        vkGetSwapchainImagesKHR(context.device, context.swapchain, &context.imageCount, NULL);
        context.v_swapchainImage = malloc(sizeof(VkImage) * context.imageCount);
        vkGetSwapchainImagesKHR(context.device, context.swapchain, &context.imageCount, context.v_swapchainImage);

        context.v_swapchainImageView = get_swapchain_image_view(context.device, context.swapchain, context.swapchainFormat, context.v_swapchainImage, context.imageCount);
        if(context.v_swapchainImageView == NULL)
                goto ERROR;


        /*
        context.renderPass = create_render_pass(context.device, context.swapchainFormat);
        if(context.renderPass == VK_NULL_HANDLE)
                goto ERROR;

        context.v_frameBuffer = create_frame_buffer(context.device, context.renderPass, context.v_swapchainImageView, context.imageCount);
        if(context.v_frameBuffer == NULL)
                goto ERROR;
                */


        context.commandPool = create_command_pool(context.device, context.s_queueFamilyIndice.graphique);

        vkGetDeviceQueue(context.device, context.s_queueFamilyIndice.graphique, 0, &context.graphiqueQueue);

        context.v_commandBuffer = create_command_buffer(context.device, context.commandPool, context.imageCount);

        fn_createSemaphore(&context);

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
                        .pEngineName = "pierrot le fou engine",
                        .engineVersion = VK_MAKE_VERSION(1,0,0),
                        .apiVersion = VK_API_VERSION_1_3
                };

        u32 glfw_extension_count = 0;
        const char** glfw_extension = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
        const char** extension = malloc(sizeof(char*) * (glfw_extension_count+gc_debugExtensionCount));

        for(int i=0; i<glfw_extension_count; i++)
        {
                printf("extension glfw %s\n", glfw_extension[i]);
                extension[i] = glfw_extension[i];
        }

#ifdef DEBUG
        for(int i=glfw_extension_count; i<glfw_extension_count+gc_debugExtensionCount; i++)
        {
                extension[i] = gcvv_debugExtensionName[i-glfw_extension_count];
        }
#endif


        u32 layer_count;
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
                        .enabledExtensionCount = glfw_extension_count+gc_debugExtensionCount,
                        .ppEnabledExtensionNames = extension,

                #ifdef DEBUG
                        .enabledLayerCount = gc_validationLayerCount,
                        .ppEnabledLayerNames = gcvv_validationLayer,
                #endif

                };

        VkResult result = vkCreateInstance(&create_info, NULL, &instance);
        if(result != VK_SUCCESS)
        {
                fprintf(stderr, "Instance creation failed %d \"%s\"\n", result, fn_getVulkanError(result));
                return VK_NULL_HANDLE;
        }

        return instance;
}

#ifdef DEBUG
VkDebugUtilsMessengerEXT create_vulkan_debug_callback(VkInstance instance, S_vulkanDebugCallbackUserArg* user_arg)
{
        VkDebugUtilsMessengerCreateInfoEXT create_info =
                {
                        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
                        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT  | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT,
                        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
                        .pfnUserCallback = (PFN_vkDebugUtilsMessengerCallbackEXT) vulkan_debug_callback,
                        .pUserData = user_arg
                };

        PFN_vkCreateDebugUtilsMessengerEXT create_debug_callback_fn = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if(create_debug_callback_fn == NULL)
                return VK_NULL_HANDLE;

        VkDebugUtilsMessengerEXT debug_callback;
        VkResult result = create_debug_callback_fn(instance, &create_info, NULL, &debug_callback);
        if(result != VK_SUCCESS)
        {
                fprintf(stderr, ANSI_RED_TEXT("Error") " : debug callback creation failed [%d] %s\n", result, fn_getVulkanError(result));
                return VK_NULL_HANDLE;
        }
        return debug_callback;
}

VKAPI_ATTR VkBool32 VKAPI_CALL vulkan_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, S_vulkanDebugCallbackUserArg* pUserData)
{
        FILE* output_stream = NULL;

        switch(messageSeverity)
        {
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                        output_stream = pUserData->verbose_stream;
                        fprintf(output_stream, ANSI_BLUE_TEXT("verbose "));
                        break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                        output_stream = pUserData->info_stream;
                        fprintf(output_stream, ANSI_BLUE_TEXT("info "));
                        break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                        output_stream = pUserData->warning_stream;
                        fprintf(output_stream, ANSI_BLUE_TEXT("warning "));
                        break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                        output_stream = pUserData->error_stream;
                        fprintf(output_stream, ANSI_BLUE_TEXT("error "));
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
                        break;
        }

        if(output_stream == NULL)
                return VK_FALSE;

        fprintf(output_stream, ANSI_RED_TEXT("validation layer") " : %s\n", pCallbackData->pMessage);

        return VK_FALSE;
}
#endif

VkPhysicalDevice find_physical_device(VkInstance instance)
{
        VkPhysicalDevice physical_device;

        u32 physical_device_count;
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

S_queueFamilyIndice find_queue_family(VkPhysicalDevice physical_device)
{
        S_queueFamilyIndice queue_family_indice =
                {
                        .graphique = -1,
                        .compute = -1,
                        .transfert = -1,
                };

        u32 queue_family_count;
        vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, NULL);
        VkQueueFamilyProperties* queue_family = malloc(sizeof(VkQueueFamilyProperties) * queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_family);

        for(int i=0; i<queue_family_count; i++)
        {
                if(queue_family[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                {
                        queue_family_indice.graphique = i;
                }

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

VkDevice create_device(VkPhysicalDevice physical_device, S_queueFamilyIndice queue_family_indice)
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

        VkPhysicalDeviceSynchronization2Features sync2Feature =
                {
                        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES,
                        .synchronization2 = VK_TRUE
                };

        VkPhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeature =
                {
                        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR,
                        .pNext = &sync2Feature,
                        .dynamicRendering = VK_TRUE
                };

        VkDeviceCreateInfo create_info =
                {
                        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                        .pNext = &dynamicRenderingFeature,
                        .pQueueCreateInfos = &device_queue_info,
                        .queueCreateInfoCount = 1,
                        .pEnabledFeatures = &device_feature,

                        .enabledExtensionCount = gc_deviceExtensionCount,
                        .ppEnabledExtensionNames = gcvv_deviceExtension,

                };

        VkResult result = vkCreateDevice(physical_device, &create_info, NULL, &device);
        if(result != VK_SUCCESS)
        {
                fprintf(stderr, "Error : physical device creation failed [%d] \"%s\"\n", result, fn_getVulkanError(result));
                return VK_NULL_HANDLE;
        }

        if(!vkGetDeviceProcAddr(device, "vkCmdBeginRendering"))
                printf("dynamique not enable \n");

        return device;
}

VkSwapchainKHR create_swapchain(VkPhysicalDevice physical_device, VkDevice device, VkSurfaceKHR surface, VkExtent2D s_swapExtent, VkFormat* swapchain_format)
{
        VkSwapchainKHR swapchain;

        VkSurfaceCapabilitiesKHR capability;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &capability);

        VkSurfaceFormatKHR* format_vec;
        VkSurfaceFormatKHR format;

        u32 format_count;
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

        u32 present_mode_count;
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
                        //.imageExtent = capability.currentExtent,
                        .imageExtent = s_swapExtent,
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
                fprintf(stderr, "Error : swap chain creation failed [%d] \"%s\"\n", result, fn_getVulkanError(result));
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
                        fprintf(stderr, "Error : image view %d creation failed [%d] \"%s\"\n", i, result, fn_getVulkanError(result));
                        return NULL;
                }
        }

        return swapchain_image_view;
}

VkRenderPass create_render_pass(VkDevice device, VkFormat format)
{
        VkRenderPass render_pass;
        VkAttachmentDescription attachment =
                {
                        .format = format,
                        .samples = VK_SAMPLE_COUNT_1_BIT,

                        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,

                        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,

                        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
                };

        VkAttachmentReference attachment_ref =
                {
                        .attachment = 0,
                        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
                };

        VkSubpassDescription subpass =
                {
                        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,

                        .colorAttachmentCount = 1,
                        .pColorAttachments = &attachment_ref,
                };

        /*VkSubpassDependency dependency =
                {
                        .srcSubpass = VK_SUBPASS_EXTERNAL,
                        .dstSubpass = 0,

                        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                        .srcAccessMask = 0,

                        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
                };*/

        VkRenderPassCreateInfo create_info =
                {
                        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
                        .attachmentCount = 1,
                        .pAttachments = &attachment,
                        .subpassCount = 1,
                        .pSubpasses = &subpass
                        //.dependencyCount = 1,
                        //.pDependencies = &dependency
                };

        VkResult result = vkCreateRenderPass(device, &create_info, NULL, &render_pass);
        if(result != VK_SUCCESS)
        {
                fprintf(stderr, "Error : render pass creation failed [%d] \"%s\"\n", result, fn_getVulkanError(result));
                return VK_NULL_HANDLE;
        }
        return render_pass;
}

VkCommandPool create_command_pool(VkDevice device, int queue_family_indice)
{
        VkCommandPool command_pool;

        VkCommandPoolCreateInfo create_info =
                {
                        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                        .queueFamilyIndex = queue_family_indice,
                };

        VkResult result = vkCreateCommandPool(device, &create_info, NULL, &command_pool);
        if(result != VK_SUCCESS)
        {
                fprintf(stderr, "Error : command pool creation failed [%d] \"%s\"\n", result, fn_getVulkanError(result));
                return VK_NULL_HANDLE;
        }
        return command_pool;
}

VkCommandBuffer* create_command_buffer(VkDevice device, VkCommandPool command_pool, int image_count)
{
        VkCommandBuffer* command_buffer = malloc(sizeof(VkCommandBuffer) * image_count);

        VkCommandBufferAllocateInfo alloc_info =
                {
                        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                        .commandPool = command_pool,
                        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                        .commandBufferCount = image_count
                };

        VkResult result = vkAllocateCommandBuffers(device, &alloc_info, command_buffer);
        if(result != VK_SUCCESS)
        {
                fprintf(stderr, "Error : command buffer creation failed [%d] \"%s\"\n", result, fn_getVulkanError(result));
                return NULL;
        }
        return command_buffer;
}

void fn_createSemaphore(S_vulkanContext* ps_context)
{
        ps_context->v_renderOverSemaphore = malloc(sizeof(VkSemaphore) * ps_context->maxImageInFlight);
        ps_context->v_presentOverSemaphore = malloc(sizeof(VkSemaphore) * ps_context->maxImageInFlight);
        ps_context->v_inFlightFence = malloc(sizeof(VkFence) * ps_context->maxImageInFlight);
        ps_context->v_imageInFlightFence = malloc(sizeof(VkFence) * ps_context->imageCount);

        VkSemaphoreCreateInfo s_semaphoreCreateInfo =
                {
                        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
                };

        VkFenceCreateInfo s_fenceCreateInfo =
                {
                        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                        .flags = VK_FENCE_CREATE_SIGNALED_BIT
                };

        for(int i=0; i<ps_context->maxImageInFlight; i++)
        {
                VkResult result = vkCreateSemaphore(ps_context->device, &s_semaphoreCreateInfo, NULL, &ps_context->v_renderOverSemaphore[i]);
                if(result != VK_SUCCESS)
                {
                        fprintf(stderr, ANSI_RED_TEXT("Vulkan error :") " Semaphore creation failed [%d] %s\n", result, fn_getVulkanError(result));
                        goto GO_END_ERROR;
                }
                result = vkCreateSemaphore(ps_context->device, &s_semaphoreCreateInfo, NULL, &ps_context->v_presentOverSemaphore[i]);
                if(result != VK_SUCCESS)
                {
                        fprintf(stderr, ANSI_RED_TEXT("Vulkan error :") " Semaphore creation failed [%d] %s\n", result, fn_getVulkanError(result));
                        goto GO_END_ERROR;
                }

                result = vkCreateFence(ps_context->device, &s_fenceCreateInfo, NULL, &ps_context->v_inFlightFence[i]);
                if(result != VK_SUCCESS)
                {
                        fprintf(stderr, ANSI_RED_TEXT("Vulkan error :") " Fence creation failed [%d] %s\n", result, fn_getVulkanError(result));
                        goto GO_END_ERROR;
                }
        }

        for(int i=0; i<ps_context->imageCount; i++)
                ps_context->v_imageInFlightFence[i] = VK_NULL_HANDLE;

        return;
//__________________________________________________

GO_END_ERROR:
        free(ps_context->v_renderOverSemaphore);
        free(ps_context->v_presentOverSemaphore);
        free(ps_context->v_inFlightFence);

        ps_context->v_renderOverSemaphore = NULL;
        ps_context->v_presentOverSemaphore = NULL;
        ps_context->v_inFlightFence = NULL;
}

void clear_context(S_vulkanContext context)
{

        printf("clear context\n");

        for(int i=0; i<context.maxImageInFlight; i++)
        {
                vkDestroySemaphore(context.device, context.v_renderOverSemaphore[i], NULL);
                vkDestroySemaphore(context.device, context.v_presentOverSemaphore[i], NULL);
                vkDestroyFence(context.device, context.v_inFlightFence[i], NULL);
        }

        free(context.v_renderOverSemaphore);
        free(context.v_presentOverSemaphore);
        free(context.v_inFlightFence);

        for(int i=0; i<context.imageCount; i++)
        {
                vkDestroyImageView(context.device, context.v_swapchainImageView[i], NULL);
        }

        vkDestroyCommandPool(context.device, context.commandPool, NULL);

        free(context.v_commandBuffer);
        free(context.v_swapchainImage);
        free(context.v_swapchainImageView);

        printf("clear context\n");

        vkDestroySwapchainKHR(context.device, context.swapchain, NULL);
        vkDestroySurfaceKHR(context.instance, context.surface, NULL);
        vkDestroyDevice(context.device, NULL);

        free(context.ps_debugCallbackArg);
        const PFN_vkDestroyDebugUtilsMessengerEXT cpfn_vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(context.instance, "vkDestroyDebugUtilsMessengerEXT");
        cpfn_vkDestroyDebugUtilsMessengerEXT(context.instance, context.debugCallback, NULL);

        vkDestroyInstance(context.instance, NULL);
}
