#pragma once
#include <vulkan/vulkan.h>

void magmatis_render(VkRenderPass render_pass, VkCommandBuffer command_buffer,
                     VkFramebuffer framebuffer, VkExtent2D extent,
                     VkPipeline pipeline, VkViewport viewport, VkRect2D scissor,
                     VkBuffer vertex_buffer, uint32_t vertex_count);

#ifndef USE_LONG_NAMES
#define render magmatis_render
#endif