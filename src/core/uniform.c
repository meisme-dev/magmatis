#include "uniform.h"
#include "buffer.h"
#include "cglm/cam.h"
#include "cglm/mat4.h"
#include "cglm/types.h"
#include "cglm/util.h"
#include "vulkan/vulkan_core.h"
#include <extra/colors.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

VkDescriptorSetLayout
magmatis_uniform_descriptor_set_layout_create(VkDevice device) {
  VkDescriptorSetLayoutBinding layout_binding;
  memset(&layout_binding, 0, sizeof(VkDescriptorSetLayoutBinding));

  layout_binding.binding = 0;
  layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  layout_binding.descriptorCount = 1;
  layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  layout_binding.pImmutableSamplers = NULL;

  VkDescriptorSetLayoutCreateInfo layout_create_info;
  memset(&layout_create_info, 0, sizeof(VkDescriptorSetLayoutCreateInfo));

  layout_create_info.sType =
      VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layout_create_info.bindingCount = 1;
  layout_create_info.pBindings = &layout_binding;

  VkDescriptorSetLayout descriptor_set_layout;
  if (vkCreateDescriptorSetLayout(device, &layout_create_info, NULL,
                                  &descriptor_set_layout) != VK_SUCCESS) {
    fprintf(stderr, "%sFailed to create descriptor set layout%s\n", RED, CLEAR);
    return NULL;
  }

  return descriptor_set_layout;
}

VkBuffer *magmatis_uniform_buffers_create(VmaAllocator allocator,
                                          VmaAllocation **allocations,
                                          uint32_t count, size_t size,
                                          VkDeviceMemory **buffer_memory,
                                          void ***mapped_buffers) {
  VkBuffer *buffers = calloc(count, sizeof(VkBuffer));
  *buffer_memory = calloc(count, sizeof(VkDeviceMemory));
  *mapped_buffers = calloc(count, sizeof(void *));
  *allocations = calloc(count, sizeof(VmaAllocation));

  if (!buffers || !buffer_memory || !mapped_buffers) {
    fprintf(stderr, "%sFailed to allocate uniform buffers%s\n", RED, CLEAR);
    return NULL;
  }

  for (size_t i = 0; i < count; i++) {
    buffers[i] = magmatis_buffer_create(
        allocator, &(*allocations)[i], size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_SHARING_MODE_EXCLUSIVE, VMA_MEMORY_USAGE_CPU_TO_GPU,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        0);

    vmaMapMemory(allocator, (*allocations)[i], &(*mapped_buffers)[i]);
  }

  return buffers;
}

void magmatis_uniform_buffer_update(VkExtent2D extent, void **mapped_buffers,
                                    uint32_t current_image) {
  static uint8_t initialized = 0;
  static struct timespec start_time;
  struct timespec current_time;

  if (!initialized) {
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    initialized = 1;
  }

  clock_gettime(CLOCK_MONOTONIC, &current_time);
  float elapsed_time = (current_time.tv_sec - start_time.tv_sec) +
                       (current_time.tv_nsec - start_time.tv_nsec) / 1e9;

  UniformBufferObject ubo;

  glm_mat4_identity(ubo.model);
  glm_rotate(ubo.model, elapsed_time * glm_rad(90.0f),
             (vec3){0.0f, 0.0f, 1.0f});
  glm_lookat((vec3){2.0f, 2.0f, 2.0f}, (vec3){0.0f, 0.0f, 0.0f},
             (vec3){0.0f, 0.0f, 1.0f}, ubo.view);
  glm_perspective(glm_rad(45.0f), (float)extent.width / (float)extent.height,
                  0.1f, 10.0f, ubo.proj);
  ubo.proj[1][1] *= -1.0f;

  memcpy(mapped_buffers[current_image], &ubo, sizeof(UniformBufferObject));
}

VkDescriptorPool
magmatis_uniform_buffer_descriptor_pool_create(VkDevice device,
                                               uint32_t count) {
  VkDescriptorPoolSize pool_size;
  memset(&pool_size, 0, sizeof(VkDescriptorPoolSize));
  pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  pool_size.descriptorCount = count;

  VkDescriptorPoolCreateInfo pool_info;
  memset(&pool_info, 0, sizeof(VkDescriptorPoolCreateInfo));
  pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.poolSizeCount = 1;
  pool_info.pPoolSizes = &pool_size;
  pool_info.maxSets = count;

  VkDescriptorPool descriptor_pool;
  if (vkCreateDescriptorPool(device, &pool_info, NULL, &descriptor_pool) !=
      VK_SUCCESS) {
    fprintf(stderr, "%sFailed to create descriptor pool%s\n", RED, CLEAR);
    return NULL;
  }

  return descriptor_pool;
}

VkDescriptorSet *magmatis_uniform_buffer_descriptor_set_create(
    VkDevice device, VkDescriptorPool pool, VkDescriptorSetLayout *layouts,
    VkBuffer *uniform_buffers, uint32_t count, size_t size) {
  VkDescriptorSetAllocateInfo alloc_info;
  memset(&alloc_info, 0, sizeof(VkDescriptorSetAllocateInfo));
  alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info.descriptorPool = pool;
  alloc_info.descriptorSetCount = count;
  alloc_info.pSetLayouts = layouts;

  VkDescriptorSet *descriptor_sets = calloc(count, sizeof(VkDescriptorSet));
  if (vkAllocateDescriptorSets(device, &alloc_info, descriptor_sets) !=
      VK_SUCCESS) {
    fprintf(stderr, "%sFailed to allocate descriptor set%s\n", RED, CLEAR);
    return NULL;
  }

  for (uint32_t i = 0; i < count; i++) {
    VkDescriptorBufferInfo buffer_info;
    memset(&buffer_info, 0, sizeof(VkDescriptorBufferInfo));
    buffer_info.buffer = uniform_buffers[i];
    buffer_info.offset = 0;
    buffer_info.range = size;

    VkWriteDescriptorSet write_descriptor_set;
    memset(&write_descriptor_set, 0, sizeof(VkWriteDescriptorSet));
    write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_descriptor_set.dstSet = descriptor_sets[i];
    write_descriptor_set.dstBinding = 0;
    write_descriptor_set.dstArrayElement = 0;
    write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write_descriptor_set.descriptorCount = 1;
    write_descriptor_set.pBufferInfo = &buffer_info;

    vkUpdateDescriptorSets(device, 1, &write_descriptor_set, 0, NULL);
  }

  return descriptor_sets;
}
