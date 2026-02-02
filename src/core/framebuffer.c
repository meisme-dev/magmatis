#include "framebuffer.h"
#include "vulkan/vulkan_core.h"
#include <stdlib.h>

VkFramebuffer *magmatis_framebuffer_new(VkDevice device,
                                        VkRenderPass render_pass,
                                        VkExtent2D extent,
                                        VkImageView *image_views,
                                        uint32_t image_count) {
  VkFramebuffer *framebuffer = calloc(image_count, sizeof(VkFramebuffer));

  if (!framebuffer) {
    return NULL;
  }

  for (uint32_t i = 0; i < image_count; i++) {
    VkImageView attachments[] = {image_views[i]};

    VkFramebufferCreateInfo framebuffer_info = {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .renderPass = render_pass,
        .attachmentCount = 1,
        .pAttachments = attachments,
        .width = extent.width,
        .height = extent.height,
        .layers = 1,
    };

    if (vkCreateFramebuffer(device, &framebuffer_info, NULL, &framebuffer[i]) !=
        VK_SUCCESS) {
      free(framebuffer);
      return NULL;
    }
  }

  return framebuffer;
}