#pragma once

#include "vulkan/vulkan_core.h"
#define GLFW_INCLUDE_VULKAN
#include "queue.h"
#include <GLFW/glfw3.h>

typedef struct Magmatis Magmatis;

struct Magmatis {
  GLFWwindow *window;
  VkInstance instance;
  VkSurfaceKHR surface;
  QueueFamilies queue_families;
  VkQueue present_queue;
  VkQueue graphics_queue;
  VkDevice device;
  VkSwapchainKHR swapchain;
  VkImage *images;
  uint32_t image_count;
  VkFormat format;
  VkExtent2D extent;
  VkImageView *image_views;
};

Magmatis *magmatis_program_new(unsigned int w, unsigned int h, char *title,
                               int enable_validation);
int magmatis_program_cleanup(struct Magmatis *program);

#ifndef USE_LONG_NAMES
#define program_new magmatis_program_new
#define program_cleanup magmatis_program_cleanup
#endif
