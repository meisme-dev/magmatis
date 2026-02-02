#pragma once
#include <vulkan/vulkan.h>

void magmatis_sync_create(VkDevice device, VkSemaphore *image_semaphores,
                          VkSemaphore *render_semaphores,
                          VkFence *in_flight_fences, uint32_t count);

#ifndef USE_LONG_NAMES
#define sync_create magmatis_sync_create
#endif
