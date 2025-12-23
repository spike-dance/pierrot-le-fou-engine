#ifndef CONTEXT_H
#define CONTEXT_H

#include <vulkan/vulkan.h>

#include <stdbool.h>

#include "error.h"

typedef struct Queue_family_indice
{
        Error_enum error;

        int graphique;
        int compute;
        int transfert;

} Queue_family_indice;

typedef struct Vulkan_context
{
        Error_enum error;

        VkInstance instance;

        VkPhysicalDevice physical_device;

        Queue_family_indice queue_family_indice;

        VkQueue graphique_queue;
        VkQueue compute_queue;
        VkQueue transfert_queue;

        VkDevice device;

        VkSurfaceKHR surface;

        VkSwapchainKHR swapchain;
        VkFormat swapchain_format;

        int image_count;
        VkImage* swapchain_image;
        VkImageView* swapchain_image_view;

        VkExtent2D swap_extent;

        VkRenderPass render_pass;

        VkFramebuffer* frame_buffer;

        VkCommandPool command_pool;
        VkCommandBuffer* command_buffer;
} Vulkan_context;

Vulkan_context create_context(GLFWwindow* window);

void clear_context(Vulkan_context context);

VkInstance create_instance();

VkPhysicalDevice find_physical_device(VkInstance instance);

bool is_physical_device_usable(VkPhysicalDevice physical_device);

Queue_family_indice find_queue_family(VkPhysicalDevice physical_device);

Queue_family_indice find_queue_family(VkPhysicalDevice physical_device);

VkDevice create_device(VkPhysicalDevice physical_device, Queue_family_indice queue_family_indice);

VkSwapchainKHR create_swapchain(VkPhysicalDevice physical_device, VkDevice device, VkSurfaceKHR surface, VkFormat* swapchain_format);

VkImageView* get_swapchain_image_view(VkDevice device, VkSwapchainKHR swapchain, VkFormat swapchain_format, VkImage* swapchain_image, int image_count);

VkRenderPass create_render_pass(VkDevice device, VkFormat format);

VkFramebuffer* create_frame_buffer(VkDevice device, VkRenderPass render_pass, VkImageView* swapchain_image_view, int image_count);

VkCommandPool create_command_pool(VkDevice device, int queue_family_indice);

VkCommandBuffer* create_command_buffer(VkDevice device, VkCommandPool command_pool, int image_count);

#endif
