#pragma once

#include <stb_image.h>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

VkImage magmatis_texture_image_create(VmaAllocator allocator,
                                      VmaAllocation *allocation,
                                      VkDevice device, uint32_t tex_width,
                                      uint32_t tex_height);

void magmatis_buffer_to_image_copy(VkDevice device, VkQueue queue,
                                   VkCommandPool command_pool, VkBuffer buffer,
                                   VkImage image, uint32_t width,
                                   uint32_t height);

void magmatis_image_layout_transition(VkImage image, VkDevice device,
                                      VkQueue queue, VkCommandPool command_pool,
                                      VkImageLayout old_layout,
                                      VkImageLayout new_layout);

VkImage magmatis_texture_create(VmaAllocator allocator,
                                VmaAllocation *image_allocation, VkQueue queue,
                                VkCommandPool command_pool, VkDevice device,
                                const char *path);

VkSampler magmatis_texture_sampler_create(VkPhysicalDevice physical_device,
                                          VkDevice device);