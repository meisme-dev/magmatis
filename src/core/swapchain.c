#include "swapchain.h"
#include "vulkan/vulkan_core.h"
#include <extra/colors.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CLAMP(x, low, high)                                                    \
  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

SwapchainInfo *magmatis_swapchain_info_query(VkPhysicalDevice device,
                                             VkSurfaceKHR surface) {
  SwapchainInfo *info = calloc(1, sizeof(SwapchainInfo));
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface,
                                            &info->capabilities);
  uint32_t format_count;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, NULL);

  if (format_count != 0) {
    void *tmp =
        realloc(info->formats, format_count * sizeof(VkSurfaceFormatKHR));
    if (tmp == NULL) {
      free(info->formats);
      free(info);

      fprintf(stderr, "%sFailed to allocate memory for formats%s\n", RED,
              CLEAR);
      return NULL;
    } else {
      info->formats = (VkSurfaceFormatKHR *)tmp;
    }

    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count,
                                         info->formats);
  }

  uint32_t present_mode_count;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface,
                                            &present_mode_count, NULL);

  if (present_mode_count != 0) {
    void *tmp = realloc(info->present_modes,
                        present_mode_count * sizeof(VkPresentModeKHR));
    if (tmp == NULL) {
      free(info->formats);
      free(info->present_modes);
      free(info);

      fprintf(stderr, "%sFailed to allocate memory for present_modes%s\n", RED,
              CLEAR);
      return NULL;
    } else {
      info->present_modes = (VkPresentModeKHR *)tmp;
    }

    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device, surface, &present_mode_count, info->present_modes);
  }

  info->formats_count = format_count;
  info->present_modes_count = present_mode_count;

  return info;
}

static VkSurfaceFormatKHR
swapchain_format_choose(const VkSurfaceFormatKHR *formats, uint32_t count) {
  for (uint32_t i = 0; i < count; i++) {
    if (formats[i].format == VK_FORMAT_B8G8R8A8_SRGB &&
        formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return formats[i];
    }
  }

  return formats[0];
}

static VkPresentModeKHR
swapchain_present_mode_choose(const VkPresentModeKHR *present_modes,
                              uint32_t count) {
  for (uint32_t i = 0; i < count; i++) {
    if (present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
      return present_modes[i];
    }
  }

  return VK_PRESENT_MODE_FIFO_KHR;
}

static VkExtent2D swapchain_extent_choose(VkSurfaceCapabilitiesKHR capabilities,
                                          uint32_t width, uint32_t height) {
  if (capabilities.currentExtent.width != UINT32_MAX) {
    return capabilities.currentExtent;
  }

  VkExtent2D extent = {
      .width = width,
      .height = height,
  };

  extent.width = CLAMP(extent.width, capabilities.minImageExtent.width,
                       capabilities.maxImageExtent.width);
  extent.height = CLAMP(extent.height, capabilities.minImageExtent.height,
                        capabilities.maxImageExtent.height);

  return extent;
}

VkSwapchainKHR magmatis_swapchain_create(VkPhysicalDevice physical_device,
                                         VkDevice logical_device,
                                         VkSurfaceKHR surface, uint32_t width,
                                         uint32_t height, VkImage **images,
                                         uint32_t *image_count,
                                         VkFormat *format, VkExtent2D *extent) {
  SwapchainInfo *info = swapchain_info_query(physical_device, surface);
  VkSurfaceFormatKHR surface_format =
      swapchain_format_choose(info->formats, info->formats_count);

  VkPresentModeKHR present_mode = swapchain_present_mode_choose(
      info->present_modes, info->present_modes_count);

  VkExtent2D chosen_extent =
      swapchain_extent_choose(info->capabilities, width, height);

  VkSwapchainCreateInfoKHR create_info = {
      .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
      .surface = surface,
      .minImageCount = info->capabilities.minImageCount + 1,
      .imageFormat = surface_format.format,
      .imageColorSpace = surface_format.colorSpace,
      .imageExtent = chosen_extent,
      .imageArrayLayers = 1,
      .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
      .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .queueFamilyIndexCount = 0,
      .pQueueFamilyIndices = NULL,
      .preTransform = info->capabilities.currentTransform,
      .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
      .presentMode = present_mode,
      .clipped = VK_TRUE,
      .oldSwapchain = VK_NULL_HANDLE,
  };

  VkSwapchainKHR swapchain;
  if (vkCreateSwapchainKHR(logical_device, &create_info, NULL, &swapchain) !=
      VK_SUCCESS) {
    fprintf(stderr, "%sFailed to create swapchain%s\n", RED, CLEAR);
    return NULL;
  }

  vkGetSwapchainImagesKHR(logical_device, swapchain, image_count, NULL);
  *images = calloc(*image_count, sizeof(VkImage));
  vkGetSwapchainImagesKHR(logical_device, swapchain, image_count, *images);

  *format = surface_format.format;
  *extent = chosen_extent;

  return swapchain;
}

#undef CLAMP
