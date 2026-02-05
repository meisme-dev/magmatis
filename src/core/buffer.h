#pragma once
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

VkBuffer magmatis_buffer_create(VmaAllocator allocator,
                                VmaAllocation *allocation, size_t size,
                                VkBufferUsageFlags buffer_usage,
                                VkSharingMode sharing_mode,
                                VmaMemoryUsage memory_usage,
                                VkMemoryPropertyFlags memory_property_flags,
                                VmaAllocationCreateFlags allocation_flags);

void magmatis_buffer_copy(VkDevice device, VkQueue queue,
                          VkCommandPool command_pool, VkBuffer src_buffer,
                          VkBuffer dst_buffer, size_t size);

VkBuffer magmatis_buffer_upload(VkDevice device, VkQueue queue,
                                VkCommandPool command_pool,
                                VmaAllocator allocator,
                                VmaAllocation *allocation, void *data,
                                VkBufferUsageFlags usage_flags, size_t size);

#ifndef USE_LONG_NAMES
#define buffer_create magmatis_buffer_create
#define buffer_copy magmatis_buffer_copy
#define buffer_upload magmatis_buffer_upload
#endif