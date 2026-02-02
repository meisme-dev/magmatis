#pragma once

#include "queue.h"
#include <vulkan/vulkan.h>

VkCommandBuffer *magmatis_command_buffer_create(VkDevice device,
                                                VkCommandPool command_pool);
VkCommandPool magmatis_command_pool_create(VkDevice device,
                                           QueueFamilies families);
void magmatis_command_buffer_begin(VkCommandBuffer command_buffer);
void magmatis_command_buffer_end(VkCommandBuffer command_buffer);

#ifndef USE_LONG_NAMES
#define command_buffer_create magmatis_command_buffer_create
#define command_buffer_begin magmatis_command_buffer_begin
#define command_buffer_end magmatis_command_buffer_end
#define command_pool_create magmatis_command_pool_create
#endif