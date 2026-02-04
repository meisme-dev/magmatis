#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan.h>

#include "command.h"
#include "device.h"
#include "framebuffer.h"
#include "image.h"
#include "instance.h"
#include "magmatis.h"
#include "memory.h"
#include "pipeline.h"
#include "swapchain.h"
#include "sync.h"
#include "vertex.h"
#include "vulkan/vulkan_core.h"
#include <core/window.h>
#include <extra/colors.h>
#include <program_info.h>
#include <vk_mem_alloc.h>

int magmatis_program_cleanup(struct Magmatis *program) {
  vkDeviceWaitIdle(program->device);

  vmaDestroyBuffer(program->vma_allocator, program->vertex_buffer,
                   program->vertex_allocation);
  vmaDestroyAllocator(program->vma_allocator);

  for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkDestroySemaphore(program->device, program->image_semaphores[i], NULL);
    vkDestroySemaphore(program->device, program->render_semaphores[i], NULL);
    vkDestroyFence(program->device, program->in_flight_fences[i], NULL);
  }
  vkDestroyCommandPool(program->device, program->command_pool, NULL);
  vkDestroyPipeline(program->device, program->pipeline, NULL);
  vkDestroyPipelineLayout(program->device, program->pipeline_layout, NULL);
  vkDestroyRenderPass(program->device, program->render_pass, NULL);

  for (uint32_t i = 0; i < program->image_count; i++) {
    vkDestroyFramebuffer(program->device, program->framebuffers[i], NULL);
    vkDestroyImageView(program->device, program->image_views[i], NULL);
  }

  vkDestroySwapchainKHR(program->device, program->swapchain, NULL);
  vkDestroyDevice(program->device, NULL);
  vkDestroySurfaceKHR(program->instance, program->surface, NULL);
  vkDestroyInstance(program->instance, NULL);
  glfwDestroyWindow(program->window);
  glfwTerminate();

  free(program);
  return EXIT_SUCCESS;
}

Magmatis *magmatis_program_new(unsigned int w, unsigned int h, char *title,
                               int enable_validation) {
  Magmatis *program = (Magmatis *)calloc(1, sizeof(Magmatis));

  int hints[] = {GLFW_CLIENT_API, GLFW_RESIZABLE};
  int values[] = {GLFW_NO_API, GLFW_TRUE};

  GLFWwindow *window =
      window_glfw_new(w, h, title, hints, values, sizeof(hints) / sizeof(int),
                      program, framebuffer_resize_callback);

  if (program == NULL) {
    fprintf(stderr, "%sMemory allocation failure%s\n", RED, CLEAR);
    return NULL;
  }

  uint32_t layers_count = 0;
  uint32_t extensions_count = 1;

  const char *layers[] = {NULL};
  const char *extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  if (enable_validation) {
    layers[0] = "VK_LAYER_KHRONOS_validation";
    layers_count++;
  }

  VkInstance instance =
      instance_create(layers, layers_count, PROGRAM_NAME, ENGINE_NAME,
                      VK_MAKE_API_VERSION(V_VARIANT, V_MAJOR, V_MINOR, V_PATCH),
                      VK_MAKE_API_VERSION(V_VARIANT, V_MAJOR, V_MINOR, V_PATCH),
                      VK_API_VERSION_1_3);

  VkSurfaceKHR surface = window_glfw_surface_create(instance, window);

  QueueFamilies queue_families;

  VkPhysicalDevice physical_device =
      device_select(surface, instance, extensions, extensions_count);

  VkDevice device = device_create(surface, physical_device, &queue_families,
                                  extensions, extensions_count);

  VmaAllocator vma_allocator = vma_create(physical_device, device, instance);

  VkQueue present_queue = queue_get(device, queue_families.present_family);
  VkQueue graphics_queue = queue_get(device, queue_families.graphics_family);

  VkImage *images = NULL;
  uint32_t image_count = 0;
  VkFormat format = VK_FORMAT_UNDEFINED;
  VkExtent2D extent = {0, 0};

  VkSwapchainKHR swapchain =
      swapchain_create(physical_device, device, surface, w, h, &images,
                       &image_count, &format, &extent);

  VkImageView *image_views =
      image_view_create(device, images, format, image_count);

  VkRenderPass render_pass = render_pass_create(device, format);

  VkPipelineShaderStageCreateInfo *shader_stages = NULL;
  PipelineInfo *pipeline_info = calloc(1, sizeof(PipelineInfo));

  pipeline_info->shader_stages = shader_stages;

  VkPipelineLayout pipeline_layout =
      pipeline_layout_create(device, extent, pipeline_info);

  VkPipeline pipeline =
      pipeline_create(device, render_pass, pipeline_layout, pipeline_info);

  VkFramebuffer *framebuffers =
      framebuffer_new(device, render_pass, extent, image_views, image_count);

  VkCommandPool command_pool = command_pool_create(device, queue_families);

  VkCommandBuffer *command_buffers =
      command_buffer_create(device, command_pool);

  VkSemaphore *image_semaphores =
      calloc(MAX_FRAMES_IN_FLIGHT, sizeof(VkSemaphore));
  VkSemaphore *render_semaphores =
      calloc(MAX_FRAMES_IN_FLIGHT, sizeof(VkSemaphore));
  VkFence *in_flight_fences = calloc(MAX_FRAMES_IN_FLIGHT, sizeof(VkFence));

  uint32_t vertex_count = 6;

  Vertex *vertices = calloc(vertex_count, sizeof(Vertex));
  vertices[0] =
      (Vertex){.position = {-0.5f, -0.5f}, .color = {1.0f, 0.0f, 0.0f}};
  vertices[1] = (Vertex){.position = {0.5f, 0.5f}, .color = {0.0f, 1.0f, 0.0f}};
  vertices[2] =
      (Vertex){.position = {-0.5f, 0.5f}, .color = {0.0f, 0.0f, 1.0f}};
  vertices[3] =
      (Vertex){.position = {0.5f, -0.5f}, .color = {1.0f, 1.0f, 1.0f}};
  vertices[4] = (Vertex){.position = {0.5f, 0.5f}, .color = {0.0f, 1.0f, 0.0f}};
  vertices[5] =
      (Vertex){.position = {-0.5f, -0.5f}, .color = {1.0f, 0.0f, 0.0f}};

  VmaAllocation vertex_allocation;

  VkBuffer vertex_buffer = magmatis_vertex_buffer_create(
      vma_allocator, &vertex_allocation, vertices, vertex_count);

  magmatis_sync_create(device, image_semaphores, render_semaphores,
                       in_flight_fences, MAX_FRAMES_IN_FLIGHT);

  program->window = window;
  program->instance = instance;
  program->device = device;
  program->surface = surface;
  program->queue_families = queue_families;
  program->present_queue = present_queue;
  program->graphics_queue = graphics_queue;
  program->swapchain = swapchain;
  program->format = format;
  program->extent = extent;
  program->image_views = image_views;
  program->image_count = image_count;
  program->pipeline_layout = pipeline_layout;
  program->pipeline_info = pipeline_info;
  program->render_pass = render_pass;
  program->pipeline = pipeline;
  program->framebuffers = framebuffers;
  program->image_semaphores = image_semaphores;
  program->render_semaphores = render_semaphores;
  program->in_flight_fences = in_flight_fences;
  program->command_buffers = command_buffers;
  program->command_pool = command_pool;
  program->physical_device = physical_device;
  program->image_index = 0;
  program->current_frame = 0;
  program->width = w;
  program->height = h;
  program->framebuffer_resized = 0;
  program->vma_allocator = vma_allocator;
  program->vertex_buffer = vertex_buffer;
  program->vertex_count = vertex_count;
  program->vertex_allocation = vertex_allocation;

  return program;
}