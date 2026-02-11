#pragma once

#include <cglm/cglm.h>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

typedef struct UniformBufferObject {
  mat4 model;
  mat4 view;
  mat4 proj;
} UniformBufferObject;

VkBuffer *magmatis_uniform_buffers_create(VmaAllocator allocator,
                                          VmaAllocation **allocation,
                                          uint32_t count, size_t size,
                                          VkDeviceMemory **buffer_memory,
                                          void ***mapped_buffers);

VkDescriptorSetLayout
magmatis_uniform_descriptor_set_layout_create(VkDevice device);

void magmatis_uniform_buffer_update(VkExtent2D extent, void **mapped_buffers,
                                    uint32_t current_image);

VkDescriptorPool magmatis_uniform_buffer_descriptor_pool_create(VkDevice device,
                                                                uint32_t count);

VkDescriptorSet *magmatis_uniform_buffer_descriptor_set_create(
    VkDevice device, VkDescriptorPool pool, VkDescriptorSetLayout *layouts,
    VkBuffer *uniform_buffers, VkImageView image_view, VkSampler sampler,
    uint32_t count, size_t size);

#ifndef USE_LONG_NAMES
#define uniform_descriptor_set_layout_create                                   \
  magmatis_uniform_descriptor_set_layout_create
#define uniform_buffers_create magmatis_uniform_buffers_create
#endif