#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

VkFramebuffer *magmatis_framebuffer_new(VkDevice device,
                                        VkRenderPass render_pass,
                                        VkExtent2D extent,
                                        VkImageView *image_views,
                                        uint32_t image_count);

void magmatis_framebuffer_resize_callback(GLFWwindow *window, int width,
                                          int height);

#ifndef USE_LONG_NAMES
#define framebuffer_new magmatis_framebuffer_new
#define framebuffer_resize_callback magmatis_framebuffer_resize_callback
#endif
