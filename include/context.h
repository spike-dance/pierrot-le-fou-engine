#ifndef CONTEXT_H
#define CONTEXT_H

#include <vulkan/vulkan.h>

#include <my_std.h>

#include "error.h"

typedef struct
{
        E_error error;

        int graphique;
        int compute;
        int transfert;

} S_queueFamilyIndice;

#ifdef DEBUG
typedef struct
{
        FILE* verbose_stream;
        FILE* info_stream;
        FILE* warning_stream;
        FILE* error_stream;
} S_vulkanDebugCallbackUserArg;
#endif

typedef struct
{
        E_error error;

        VkInstance instance;
        VkDebugUtilsMessengerEXT debugCallback;
        S_vulkanDebugCallbackUserArg* ps_debugCallbackArg;

        VkPhysicalDevice physicalDevice;

        S_queueFamilyIndice s_queueFamilyIndice;

        VkQueue graphiqueQueue;
        VkQueue computeQueue;
        VkQueue transfertQueue;

        VkDevice device;

        VkSurfaceKHR surface;

        VkSwapchainKHR swapchain;
        VkFormat swapchainFormat;

        u32 imageCount;
        VkImage* v_swapchainImage;
        VkImageView* v_swapchainImageView;

        VkExtent2D s_swapExtent;

        VkRenderPass renderPass;

        VkFramebuffer* v_frameBuffer;

        VkCommandPool commandPool;
        VkCommandBuffer* v_commandBuffer;

        u32 maxImageInFlight;
        VkSemaphore* v_renderOverSemaphore;
        VkSemaphore* v_presentOverSemaphore;
        VkFence* v_inFlightFence;
        VkFence* v_imageInFlightFence;
} S_vulkanContext;

S_vulkanContext create_context(GLFWwindow* window);

void clear_context(S_vulkanContext context);

VkInstance create_instance();

VkPhysicalDevice find_physical_device(VkInstance instance);

bool is_physical_device_usable(VkPhysicalDevice physical_device);

S_queueFamilyIndice find_queue_family(VkPhysicalDevice physical_device);

S_queueFamilyIndice find_queue_family(VkPhysicalDevice physical_device);

VkDevice create_device(VkPhysicalDevice physical_device, S_queueFamilyIndice queue_family_indice);

VkSwapchainKHR create_swapchain(VkPhysicalDevice physical_device, VkDevice device, VkSurfaceKHR surface, VkFormat* swapchain_format);

VkImageView* get_swapchain_image_view(VkDevice device, VkSwapchainKHR swapchain, VkFormat swapchain_format, VkImage* swapchain_image, int image_count);

VkRenderPass create_render_pass(VkDevice device, VkFormat format);

VkFramebuffer* create_frame_buffer(VkDevice device, VkRenderPass render_pass, VkImageView* swapchain_image_view, int image_count);

VkCommandPool create_command_pool(VkDevice device, int queue_family_indice);

VkCommandBuffer* create_command_buffer(VkDevice device, VkCommandPool command_pool, int image_count);

void fn_createSemaphore(S_vulkanContext* ps_context);

VkDebugUtilsMessengerEXT create_vulkan_debug_callback(VkInstance instance, S_vulkanDebugCallbackUserArg* user_arg);

#ifdef DEBUG
VKAPI_ATTR VkBool32 VKAPI_CALL vulkan_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, S_vulkanDebugCallbackUserArg* pUserData);
#endif

#endif
