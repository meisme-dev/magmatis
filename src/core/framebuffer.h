#pragma once

#include <vulkan/vulkan.h>

VkFramebuffer *magmatis_framebuffer_new(VkDevice device,
                                        VkRenderPass render_pass,
                                        VkExtent2D extent,
                                        VkImageView *image_views,
                                        uint32_t image_count);

#ifndef USE_LONG_NAMES
#define framebuffer_new magmatis_framebuffer_new
#endif
