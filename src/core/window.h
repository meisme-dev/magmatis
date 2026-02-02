#pragma once
#define GLFW_INCLUDE_VULKAN
#include "magmatis.h"
#include <GLFW/glfw3.h>
#include <stdbool.h>

GLFWwindow *magmatis_window_glfw_new(int w, int h, char *title, int *hints,
                                     int *values, int hint_count);

const char **magmatis_window_extensions_get(uint32_t *required_extension_count);

int magmatis_window_glfw_event_loop_run(Magmatis *program,
                                        int (*fun)(Magmatis *));

VkSurfaceKHR magmatis_window_glfw_surface_create(VkInstance instance,
                                                 GLFWwindow *window);

#ifndef USE_LONG_NAMES
#define window_glfw_new magmatis_window_glfw_new

#define window_extensions_get magmatis_window_extensions_get

#define window_glfw_event_loop_run magmatis_window_glfw_event_loop_run

#define window_glfw_surface_create magmatis_window_glfw_surface_create
#endif