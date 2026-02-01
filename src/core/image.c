#include "image.h"

#include <extra/colors.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

VkImageView *magmatis_image_view_create(VkDevice device, VkImage *images,
                                        VkFormat format, uint32_t image_count) {
  VkImageView *swapchain_image_views = calloc(image_count, sizeof(VkImageView));
  for (uint32_t i = 0; i < image_count; i++) {
    VkImageViewCreateInfo image_view_create_info;
    memset(&image_view_create_info, 0, sizeof(image_view_create_info));

    image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_create_info.image = images[i];
    image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    image_view_create_info.format = format;
    image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_create_info.subresourceRange.aspectMask =
        VK_IMAGE_ASPECT_COLOR_BIT;
    image_view_create_info.subresourceRange.baseMipLevel = 0;
    image_view_create_info.subresourceRange.levelCount = 1;
    image_view_create_info.subresourceRange.baseArrayLayer = 0;
    image_view_create_info.subresourceRange.layerCount = 1;

    if (vkCreateImageView(device, &image_view_create_info, NULL,
                          &swapchain_image_views[i]) != VK_SUCCESS) {
      fprintf(stderr, "%sFailed to create image view%s\n", RED, CLEAR);
      return NULL;
    }
  }

  return swapchain_image_views;
}
