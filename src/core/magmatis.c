#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan.h>

#include "device.h"
#include "image.h"
#include "instance.h"
#include "magmatis.h"
#include "swapchain.h"
#include "vulkan/vulkan_core.h"
#include <core/window.h>
#include <extra/colors.h>
#include <program_info.h>

int magmatis_program_cleanup(struct Magmatis *program) {
  vkDeviceWaitIdle(program->device);
  for (uint32_t i = 0; i < program->image_count; i++) {
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
  Magmatis *program = (Magmatis *)malloc(sizeof(Magmatis));

  int hints[] = {GLFW_CLIENT_API, GLFW_RESIZABLE};
  int values[] = {GLFW_NO_API, GLFW_FALSE};

  GLFWwindow *window =
      window_glfw_new(w, h, title, hints, values, sizeof(hints) / sizeof(int));

  if (program == NULL) {
    fprintf(stderr, "%sMemory allocation failure%s", RED, CLEAR);
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
      magmatis_image_view_create(device, images, format, image_count);

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

  return program;
}