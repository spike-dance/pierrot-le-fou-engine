#include <vulkan/vulkan.h>

#include "graphique_pipeline.h"
#include "shader.h"

void create_vert_frag_graphique_pipeline(VkDevice device, char* vertex_file_path, char* fragment_file_path)
{
        VkShaderModule vertex_shader_module = create_shader_module(device, vertex_file_path);
        VkShaderModule fragment_shader_module = create_shader_module(device, fragment_file_path);

        VkPipelineShaderStageCreateInfo vertex_shader_stage_info =
                {
                        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                        .stage = VK_SHADER_STAGE_VERTEX_BIT,
                        .module = vertex_shader_module,
                        .pName = "main"
                };

        VkPipelineShaderStageCreateInfo fragment_shader_stage_info =
                {
                        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                        .module = fragment_shader_module,
                        .pName = "main"
                };

        VkPipelineShaderStageCreateInfo shader_stage_info[] = {vertex_shader_stage_info, fragment_shader_stage_info};

        VkPipelineVertexInputStateCreateInfo vertex_input_info =
                {
                        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
                        .vertexBindingDescriptionCount = 0,
                        .pVertexBindingDescriptions = NULL,
                        .vertexAttributeDescriptionCount = 0,
                        .pVertexAttributeDescriptions = NULL,
                };

        VkPipelineInputAssemblyStateCreateInfo input_assembly =
                {
                        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
                        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                        .primitiveRestartEnable = VK_FALSE
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

        VkRect2D scissor =
                {
                        .offset = {0,0},
                        .extent = {800,500}
                };

        VkPipelineViewportStateCreateInfo viewport_state =
                {
                        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
                        .viewportCount = 1,
                        .pViewports = &viewport,
                        .scissorCount = 1,
                        .pScissors = &scissor
                };

        VkPipelineRasterizationStateCreateInfo rasterizer =
                {
                        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
                        .depthClampEnable = VK_FALSE,

                        .rasterizerDiscardEnable = VK_FALSE,

                        .polygonMode = VK_POLYGON_MODE_FILL,

                        .lineWidth = 1.0f,

                        .cullMode = VK_CULL_MODE_BACK_BIT,
                        .frontFace = VK_FRONT_FACE_CLOCKWISE,

                        .depthBiasEnable = VK_FALSE,
                        .depthBiasConstantFactor = 0.0f,
                        .depthBiasClamp = 0.0f,
                        .depthBiasSlopeFactor = 0.0f
                };

        VkPipelineMultisampleStateCreateInfo multisampling =
                {
                        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
                        .sampleShadingEnable = VK_FALSE,
                        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
                        .minSampleShading = 1.0f,
                        .pSampleMask = NULL,
                        .alphaToCoverageEnable = VK_FALSE,
                        .alphaToOneEnable = VK_FALSE
                };

        VkPipelineColorBlendAttachmentState color_blend_attachment =
                {
                        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
                        .blendEnable = VK_FALSE,
                        .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
                        .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
                        .colorBlendOp = VK_BLEND_OP_ADD,
                        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
                        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
                        .alphaBlendOp = VK_BLEND_OP_ADD

                        .blendEnable = VK_TRUE,
                        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
                        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
                        .colorBlendOp = VK_BLEND_OP_ADD,
                        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
                        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
                        .alphaBlendOp = VK_BLEND_OP_ADD,
                };

        VkPipelineColorBlendStateCreateInfo color_blend =
                {
                        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
                        .logicOpEnable = VK_FALSE,
                        .logicOp = VK_LOGIC_OP_COPY,
                        .attachmentCount = 1,
                        .pAttachments = &color_blend_attachment,
                        .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f};
                };

        VkDynamicState dynamic_state[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_LINE_WIDTH};
        VkPipelineDynamicStateCreateInfo dynamic_state =
                {
                        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
                        .dynamicStateCount = 2,
                        .pDynamicStates = dynamic_state
                };

        VkPipelineLayoutCreateInfo pipeline_layout_info =
                {
                        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO
                };

        VkPipelineLayout pipeline_layout;

        VkResult result = vkCreatePipelineLayout(device, &pipeline_layout_info, NULL, &pipeline_layout);
        if(result != VK_SUCCESS)
        {
                fprintf(stderr, "Error : pipeline layout creation failed [%d] \"%s\"\n", result, get_vulkan_error(result));
                return;
        }

        vkDestroyShaderModule(device, fragment_shader_module, NULL);
        vkDestroyShaderModule(device, vertex_shader_module, NULL);
}
