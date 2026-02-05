#pragma once
#include <vulkan/vulkan.h>

void magmatis_render(VkRenderPass render_pass, VkCommandBuffer command_buffer,
                     VkFramebuffer framebuffer, VkExtent2D extent,
                     VkPipeline pipeline, VkViewport viewport, VkRect2D scissor,
                     VkBuffer vertex_buffer, VkBuffer index_buffer,
                     VkDescriptorSet *descriptor_sets, uint32_t index_size,
                     VkPipelineLayout pipeline_layout, uint32_t current_frame);

#ifndef USE_LONG_NAMES
#define render magmatis_render
#endif