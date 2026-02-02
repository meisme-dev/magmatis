#include "command.h"
#include "magmatis.h"
#include "vulkan/vulkan_core.h"
#include <extra/colors.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

VkCommandPool magmatis_command_pool_create(VkDevice device,
                                           QueueFamilies families) {
  VkCommandPoolCreateInfo create_info;
  memset(&create_info, 0, sizeof(create_info));
  create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  create_info.queueFamilyIndex = families.graphics_family;
  create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

  VkCommandPool command_pool;
  if (vkCreateCommandPool(device, &create_info, NULL, &command_pool) !=
      VK_SUCCESS) {
    fprintf(stderr, "%sFailed to create command pool%s\n", RED, CLEAR);
    return NULL;
  }

  return command_pool;
}

VkCommandBuffer *magmatis_command_buffer_create(VkDevice device,
                                                VkCommandPool command_pool) {
  VkCommandBufferAllocateInfo alloc_info;
  memset(&alloc_info, 0, sizeof(alloc_info));
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.commandPool = command_pool;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

  VkCommandBuffer *command_buffers =
      calloc(MAX_FRAMES_IN_FLIGHT, sizeof(VkCommandBuffer));
  if (vkAllocateCommandBuffers(device, &alloc_info, command_buffers) !=
      VK_SUCCESS) {
    fprintf(stderr, "%sFailed to allocate command buffer%s\n", RED, CLEAR);
    return NULL;
  }

  return command_buffers;
}

void magmatis_command_buffer_begin(VkCommandBuffer command_buffer) {
  VkCommandBufferBeginInfo begin_info;
  memset(&begin_info, 0, sizeof(begin_info));
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS) {
    fprintf(stderr, "%sFailed to begin recording command buffer%s\n", RED,
            CLEAR);
  }
}

void magmatis_command_buffer_end(VkCommandBuffer command_buffer) {
  if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
    fprintf(stderr, "%sFailed to record command buffer%s\n", RED, CLEAR);
  }
}
