#ifndef GRAPHIQUE_PIPELINE_H
#define GRAPHIQUE_PIPELINE_H

#include <vulkan/vulkan.h>

typedef struct
{
        VkPipeline pipeline;
        VkPipelineLayout layout;
} Graphique_pipeline;

Graphique_pipeline create_vert_frag_graphique_pipeline(VkDevice device, VkFormat swapchainFormat, char* vertex_file_path, char* fragment_file_path);

void clear_graphique_pipeline(VkDevice device, Graphique_pipeline graphique_pipeline);

#endif
