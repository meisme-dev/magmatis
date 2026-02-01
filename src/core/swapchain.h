#pragma once

#include <vulkan/vulkan.h>

typedef struct SwapchainInfo {
  VkSurfaceCapabilitiesKHR capabilities;
  VkSurfaceFormatKHR *formats;
  VkPresentModeKHR *present_modes;
  size_t formats_count;
  size_t present_modes_count;
} SwapchainInfo;

SwapchainInfo *magmatis_swapchain_info_query(VkPhysicalDevice device,
                                             VkSurfaceKHR surface);

VkSwapchainKHR magmatis_swapchain_create(VkPhysicalDevice physical_device,
                                         VkDevice logical_device,
                                         VkSurfaceKHR surface, uint32_t width,
                                         uint32_t height, VkImage **images,
                                         uint32_t *image_count,
                                         VkFormat *format, VkExtent2D *extent);

#ifndef USE_LONG_NAMES
#define swapchain_info_query magmatis_swapchain_info_query
#define swapchain_create magmatis_swapchain_create
#endif