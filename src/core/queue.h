#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

#define MAGMATIS_QUEUE_FAMILY_COUNT 2

typedef struct QueueFamilies {
  uint32_t graphics_family;
  uint32_t present_family;
} QueueFamilies;

bool magmatis_queue_is_complete(QueueFamilies *queue);

QueueFamilies magmatis_queue_families_find(VkPhysicalDevice device,
                                           VkSurfaceKHR surface);

VkQueue magmatis_queue_get(VkDevice device, uint32_t family_index);

#ifndef USE_LONG_NAMES
#define queue_is_complete magmatis_queue_is_complete
#define queue_families_find magmatis_queue_families_find
#define queue_get magmatis_queue_get
#endif