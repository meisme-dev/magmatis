#pragma once

#include <vulkan/vulkan.h>

VkImageView *magmatis_image_view_create(VkDevice device, VkImage *images,
                                        VkFormat format, uint32_t image_count);

#ifndef USE_LONG_NAMES
#define image_view_create magmatis_image_view_create
#endif