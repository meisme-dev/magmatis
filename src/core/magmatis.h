#pragma once

#include "vulkan/vulkan_core.h"
#define GLFW_INCLUDE_VULKAN
#include "pipeline.h"
#include "queue.h"
#include <GLFW/glfw3.h>

#define MAX_FRAMES_IN_FLIGHT 2

typedef struct Magmatis {
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
  VkPipelineLayout pipeline_layout;
  VkRenderPass render_pass;
  PipelineInfo *pipeline_info;
  VkPipeline pipeline;
  VkFramebuffer *framebuffers;
  VkSemaphore *image_semaphores;
  VkSemaphore *render_semaphores;
  VkFence *in_flight_fences;
  VkCommandBuffer *command_buffers;
  VkCommandPool command_pool;
  uint32_t image_index;
  uint32_t current_frame;
} Magmatis;

Magmatis *magmatis_program_new(unsigned int w, unsigned int h, char *title,
                               int enable_validation);
int magmatis_program_cleanup(struct Magmatis *program);

#ifndef USE_LONG_NAMES
#define program_new magmatis_program_new
#define program_cleanup magmatis_program_cleanup
#endif
