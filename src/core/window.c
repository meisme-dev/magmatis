#include "window.h"
#include <extra/colors.h>
#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan.h>

static void magmatis_window_glfw_error_callback(int error,
                                                const char *description) {
  fprintf(stderr, "GLFW Error: %s, code: %d\n", description, error);
}

GLFWwindow *magmatis_window_glfw_new(int w, int h, char *title, int *hints,
                                     int *values, int hint_count) {
  glfwSetErrorCallback(magmatis_window_glfw_error_callback);

  if (!glfwInit()) {
    fprintf(stderr, "%sFailed to initialize glfw%s\n", RED, CLEAR);
    return NULL;
  }

  for (int i = 0; i < hint_count; i++) {
    glfwWindowHint(hints[i], values[i]);
  }

  GLFWwindow *window = glfwCreateWindow(w, h, title, NULL, NULL);
  return window;
}

const char **
magmatis_window_extensions_get(uint32_t *required_extension_count) {
  return glfwGetRequiredInstanceExtensions(required_extension_count);
}

bool magmatis_window_glfw_event_loop_run(GLFWwindow *window) {
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  }
  return true;
}

VkSurfaceKHR magmatis_window_glfw_surface_create(VkInstance instance,
                                                 GLFWwindow *window) {
  VkSurfaceKHR surface;

  if (glfwCreateWindowSurface(instance, window, NULL, &surface) != VK_SUCCESS) {
    fprintf(stderr, "%sFailed to create window surface%s\n", RED, CLEAR);
    return NULL;
  }

  return surface;
}