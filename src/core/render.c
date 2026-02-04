#include "render.h"
#include "vulkan/vulkan_core.h"

#include <string.h>

void magmatis_render(VkRenderPass render_pass, VkCommandBuffer command_buffer,
                     VkFramebuffer framebuffer, VkExtent2D extent,
                     VkPipeline pipeline, VkViewport viewport, VkRect2D scissor,
                     VkBuffer vertex_buffer, uint32_t vertex_count) {

  VkClearValue clear_color = {{{0.0f, 0.0f, 0.0f, 1.0f}}};

  VkRenderPassBeginInfo render_pass_begin_info;
  memset(&render_pass_begin_info, 0, sizeof(render_pass_begin_info));

  render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  render_pass_begin_info.renderPass = render_pass;
  render_pass_begin_info.framebuffer = framebuffer;
  render_pass_begin_info.renderArea.offset.x = 0;
  render_pass_begin_info.renderArea.offset.y = 0;
  render_pass_begin_info.renderArea.extent = extent;
  render_pass_begin_info.clearValueCount = 1;
  render_pass_begin_info.pClearValues = &clear_color;

  vkCmdBeginRenderPass(command_buffer, &render_pass_begin_info,
                       VK_SUBPASS_CONTENTS_INLINE);

  vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

  VkBuffer vertex_buffers[1] = {vertex_buffer};
  VkDeviceSize offsets[1] = {0};

  vkCmdBindVertexBuffers(command_buffer, 0, 1, vertex_buffers, offsets);

  vkCmdSetViewport(command_buffer, 0, 1, &viewport);
  vkCmdSetScissor(command_buffer, 0, 1, &scissor);
  vkCmdDraw(command_buffer, vertex_count, 1, 0, 0);

  vkCmdEndRenderPass(command_buffer);
}