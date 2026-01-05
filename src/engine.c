#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#include <my_std.h>

#include "engine.h"
#include "context.h"
#include "graphique_pipeline.h"

E_main fn_engineLoop()
{
        E_main returnValue = END;

        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        GLFWwindow* window = glfwCreateWindow(800, 500, "Scene", NULL, NULL);

        S_vulkanContext s_context = create_context(window);
        if(s_context.error != NO_ERROR)
        {
                returnValue = END;
                goto GO_END_WINDOW_CREATED;
        }


        Graphique_pipeline graphique_pipeline = create_vert_frag_graphique_pipeline(s_context.device, s_context.renderPass, "test-shader/vert.spv", "test-shader/frag.spv");



















        for(int i=0; i<s_context.imageCount; i++)
        {
                VkCommandBufferBeginInfo begin_info =
                        {
                                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                                .flags = 0,
                                .pInheritanceInfo = NULL
                        };
                vkBeginCommandBuffer(s_context.v_commandBuffer[i], &begin_info);

                VkClearValue clear_color = {0};
                if(i==0)
                clear_color.color.float32[0] = 1.0f;
                if(i==1)
                clear_color.color.float32[1]= 1.0f;
                if(i==2)
                clear_color.color.float32[2]= 1.0f;
                VkRenderPassBeginInfo render_pass_info =
                        {
                                .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                                .renderPass = s_context.renderPass,
                                .framebuffer = s_context.v_frameBuffer[i],
                                .renderArea =
                                {
                                        .offset = {0,0},
                                        .extent = {800,500}
                                },
                                .clearValueCount = 1,
                                .pClearValues = &clear_color,

                        };

                VkViewport viewport =
                        {
                                .x = 0.0f,
                                .y = 0.0f,

                                .width = 800.0f,
                                .height = 500.0f,
                                .minDepth = 0.0f,
                                .maxDepth = 1.0f
                        };

                vkCmdSetViewport(s_context.v_commandBuffer[i], 0, 1, &viewport);
                vkCmdBeginRenderPass(s_context.v_commandBuffer[i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
                vkCmdBindPipeline(s_context.v_commandBuffer[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphique_pipeline.pipeline);
                vkCmdDraw(s_context.v_commandBuffer[i], 3, 1, 0, 0);
                vkCmdEndRenderPass(s_context.v_commandBuffer[i]);
                vkEndCommandBuffer(s_context.v_commandBuffer[i]);
        }













        u32 semaphoreIndex = 0;
        bool running = true;
        while(!glfwWindowShouldClose(window))
        {








                u32 swapchainImageIndex;
                vkAcquireNextImageKHR(s_context.device, s_context.swapchain, UINT64_MAX, s_context.v_presentOverSemaphore[semaphoreIndex], VK_NULL_HANDLE, &swapchainImageIndex);






                if(s_context.v_imageInFlightFence[swapchainImageIndex] != VK_NULL_HANDLE)
                        vkWaitForFences(s_context.device, 1, &s_context.v_imageInFlightFence[swapchainImageIndex], VK_TRUE, UINT64_MAX);

                s_context.v_imageInFlightFence[swapchainImageIndex] = s_context.v_inFlightFence[semaphoreIndex];





                VkSemaphore wait_semaphore[] = {s_context.v_presentOverSemaphore[semaphoreIndex]};
                VkPipelineStageFlags wait_stage[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
                VkSemaphore signal_semaphore[] = {s_context.v_renderOverSemaphore[semaphoreIndex]};
                VkSubmitInfo submit_info =
                        {
                                .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                .waitSemaphoreCount = 1,
                                .pWaitSemaphores = wait_semaphore,
                                .pWaitDstStageMask = wait_stage,

                                .commandBufferCount = 1,
                                .pCommandBuffers = &s_context.v_commandBuffer[swapchainImageIndex],

                                .signalSemaphoreCount = 1,
                                .pSignalSemaphores = signal_semaphore,
                        };

                vkResetFences(s_context.device, 1, &
                VkResult result = vkQueueSubmit(s_context.graphiqueQueue, 1, &submit_info, VK_NULL_HANDLE);
                if(result != VK_SUCCESS)
                {
                        fprintf(stderr, "Error : command buffer submition failed [%d] \"%s\"\n", result, fn_getVulkanError(result));
                }

                VkPresentInfoKHR present_info =
                        {
                                .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                                .waitSemaphoreCount = 1,
                                .pWaitSemaphores = signal_semaphore,

                                .swapchainCount = 1,
                                .pSwapchains = &s_context.swapchain,
                                .pImageIndices = &swapchainImageIndex,

                                .pResults = NULL
                        };

                vkQueuePresentKHR(s_context.graphiqueQueue, &present_info);
                vkQueueWaitIdle(s_context.graphiqueQueue);
                glfwPollEvents();

                semaphoreIndex = (swapchainImageIndex+1) % 2;
        }

        vkDeviceWaitIdle(s_context.device);

        clear_graphique_pipeline(s_context.device, graphique_pipeline);

        clear_context(s_context);

GO_END_WINDOW_CREATED:
        glfwDestroyWindow(window);

        return returnValue;
}
