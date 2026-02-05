#pragma once

#include "pipeline.h"
#include "queue.h"
#include "vertex.h"
#include "vulkan/vulkan_core.h"
#include <stdint.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vk_mem_alloc.h>

#define MAX_FRAMES_IN_FLIGHT 2

typedef struct Magmatis {
  GLFWwindow *window;
  VkInstance instance;
  VkSurfaceKHR surface;
  QueueFamilies queue_families;
  VkQueue present_queue;
  VkQueue graphics_queue;
  VkPhysicalDevice physical_device;
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
  uint32_t width;
  uint32_t height;
  uint8_t framebuffer_resized;
  VmaAllocator vma_allocator;
  VkBuffer vertex_buffer;
  VkBuffer index_buffer;
  uint32_t vertex_count;
  uint32_t index_count;
  VmaAllocation vertex_allocation;
  VmaAllocation index_allocation;
  VmaAllocation *uniform_allocations;
  VkDescriptorSetLayout *descriptor_set_layouts;
  VkBuffer *uniform_buffers;
  void *mapped_uniform_buffers;
  uint32_t uniform_buffer_count;
  VkDescriptorPool descriptor_pool;
  VkDescriptorSet *descriptor_sets;
} Magmatis;

Magmatis *magmatis_program_new(unsigned int w, unsigned int h, char *title,
                               int enable_validation);
int magmatis_program_cleanup(struct Magmatis *program);

#ifndef USE_LONG_NAMES
#define program_new magmatis_program_new
#define program_cleanup magmatis_program_cleanup
#endif
