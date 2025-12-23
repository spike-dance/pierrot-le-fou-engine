#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "engine.h"
#include "context.h"
#include "graphique_pipeline.h"

Main_state engine_loop()
{
        Main_state return_value = END;

        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        GLFWwindow* window = glfwCreateWindow(800, 500, "Scene", NULL, NULL);

        Vulkan_context context = create_context(window);

        Graphique_pipeline graphique_pipeline = create_vert_frag_graphique_pipeline(context.device, context.render_pass, "test-shader/vert.spv", "test-shader/frag.spv");









        VkSemaphore image_available_semaphore;
        VkSemaphore render_over_semaphore;

        VkSemaphoreCreateInfo semaphore_create_info =
                {
                        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
                };

        VkResult result = vkCreateSemaphore(context.device, &semaphore_create_info, NULL, &image_available_semaphore);
        result = vkCreateSemaphore(context.device, &semaphore_create_info, NULL, &render_over_semaphore);











        for(int i=0; i<context.image_count; i++)
        {
                VkCommandBufferBeginInfo begin_info =
                        {
                                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                                .flags = 0,
                                .pInheritanceInfo = NULL
                        };
                vkBeginCommandBuffer(context.command_buffer[i], &begin_info);

                VkClearValue clear_color = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
                VkRenderPassBeginInfo render_pass_info =
                        {
                                .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                                .renderPass = context.render_pass,
                                .framebuffer = context.frame_buffer[i],
                                .renderArea =
                                {
                                        .offset = {0,0},
                                        .extent = {800,500}
                                },
                                .clearValueCount = 1,
                                .pClearValues = &clear_color,

                        };

                vkCmdBeginRenderPass(context.command_buffer[i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
                vkCmdBindPipeline(context.command_buffer[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphique_pipeline.pipeline);
                vkCmdDraw(context.command_buffer[i], 3, 1, 0, 0);
                vkCmdEndRenderPass(context.command_buffer[i]);
                vkEndCommandBuffer(context.command_buffer[i]);
        }













        bool running = true;
        while(!glfwWindowShouldClose(window))
        {
                int swapchain_image_index;
                vkAcquireNextImageKHR(context.device, context.swapchain, UINT64_MAX, image_available_semaphore, VK_NULL_HANDLE, &swapchain_image_index);

                VkSemaphore wait_semaphore[] = {image_available_semaphore};
                VkPipelineStageFlags wait_stage[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
                VkSemaphore signal_semaphore[] = {render_over_semaphore};
                VkSubmitInfo submit_info =
                        {
                                .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                .waitSemaphoreCount = 1,
                                .pWaitSemaphores = wait_semaphore,
                                .pWaitDstStageMask = wait_stage,

                                .commandBufferCount = 1,
                                .pCommandBuffers = &context.command_buffer[swapchain_image_index],

                                .signalSemaphoreCount = 1,
                                .pSignalSemaphores = signal_semaphore,
                        };

                result = vkQueueSubmit(context.graphique_queue, 1, &submit_info, VK_NULL_HANDLE);
                if(result != VK_SUCCESS)
                {
                        fprintf(stderr, "Error : command buffer submition failed [%d] \"%s\"\n", result, get_vulkan_error(result));
                }

                VkPresentInfoKHR present_info =
                        {
                                .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                                .waitSemaphoreCount = 1,
                                .pWaitSemaphores = signal_semaphore,

                                .swapchainCount = 1,
                                .pSwapchains = &context.swapchain,
                                .pImageIndices = &swapchain_image_index,

                                .pResults = NULL
                        };

                vkQueuePresentKHR(context.graphique_queue, &present_info);
                glfwPollEvents();
        }

        auto a=context.swapchain_image_view[0];

        vkDestroySemaphore(context.device, image_available_semaphore, NULL);
        vkDestroySemaphore(context.device, render_over_semaphore, NULL);
        clear_graphique_pipeline(context.device, graphique_pipeline);
        clear_context(context);
        glfwDestroyWindow(window);

        return return_value;
}
