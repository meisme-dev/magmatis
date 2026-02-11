#include "buffer.h"
#include "command.h"
#include "vulkan/vulkan_core.h"
#include <extra/colors.h>
#include <stdio.h>
#include <string.h>

VkBuffer magmatis_buffer_create(VmaAllocator allocator,
                                VmaAllocation *allocation, size_t size,
                                VkBufferUsageFlags buffer_usage,
                                VkSharingMode sharing_mode,
                                VmaMemoryUsage memory_usage,
                                VkMemoryPropertyFlags memory_property_flags,
                                VmaAllocationCreateFlags allocation_flags) {
  VkBufferCreateInfo buffer_info;
  memset(&buffer_info, 0, sizeof(buffer_info));
  buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_info.size = size;
  buffer_info.usage = buffer_usage;
  buffer_info.sharingMode = sharing_mode;

  VkBuffer buffer;

  VmaAllocationCreateInfo allocation_create_info;
  memset(&allocation_create_info, 0, sizeof(allocation_create_info));
  allocation_create_info.usage = memory_usage;
  allocation_create_info.requiredFlags = memory_property_flags;
  allocation_create_info.flags = allocation_flags;

  if (vmaCreateBuffer(allocator, &buffer_info, &allocation_create_info, &buffer,
                      allocation, NULL) != VK_SUCCESS) {
    fprintf(stderr, "%sFailed to create vertex buffer%s\n", RED, CLEAR);
    return NULL;
  }

  return buffer;
}

void magmatis_buffer_copy(VkDevice device, VkQueue queue,
                          VkCommandPool command_pool, VkBuffer src_buffer,
                          VkBuffer dst_buffer, size_t size) {
  VkCommandBuffer *command_buffer =
      magmatis_command_buffer_create(device, command_pool);

  magmatis_command_buffer_begin(*command_buffer);

  VkBufferCopy copy_region;
  memset(&copy_region, 0, sizeof(copy_region));
  copy_region.size = size;
  vkCmdCopyBuffer(*command_buffer, src_buffer, dst_buffer, 1, &copy_region);

  magmatis_command_buffer_end(*command_buffer);

  VkSubmitInfo submit_info;
  memset(&submit_info, 0, sizeof(submit_info));
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = command_buffer;

  if (vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE) != VK_SUCCESS) {
    fprintf(stderr, "%sFailed to submit queue%s\n", RED, CLEAR);
    return;
  }

  vkQueueWaitIdle(queue);
  vkFreeCommandBuffers(device, command_pool, 1, command_buffer);
}

VkBuffer magmatis_buffer_upload(VkDevice device, VkQueue queue,
                                VkCommandPool command_pool,
                                VmaAllocator allocator,
                                VmaAllocation *allocation, void *data,
                                VkBufferUsageFlags usage_flags, size_t size) {

  VmaAllocation src_allocation;

  VkBuffer src_buffer = magmatis_buffer_create(
      allocator, &src_allocation, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      VK_SHARING_MODE_EXCLUSIVE, VMA_MEMORY_USAGE_CPU_TO_GPU,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
      0);

  void *mapped_data;
  vmaMapMemory(allocator, src_allocation, &mapped_data);
  memcpy(mapped_data, data, size);
  vmaUnmapMemory(allocator, src_allocation);

  VkBuffer dst_buffer = magmatis_buffer_create(
      allocator, allocation, size,
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage_flags, VK_SHARING_MODE_EXCLUSIVE,
      VMA_MEMORY_USAGE_GPU_ONLY, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 0);

  magmatis_buffer_copy(device, queue, command_pool, src_buffer, dst_buffer,
                       size);

  vkDeviceWaitIdle(device);
  vkDestroyBuffer(device, src_buffer, NULL);
  vmaFreeMemory(allocator, src_allocation);

  return dst_buffer;
}