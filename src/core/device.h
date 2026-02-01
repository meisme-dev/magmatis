#pragma once
#include "queue.h"
#include <vulkan/vulkan.h>

VkDevice magmatis_device_create(VkSurfaceKHR surface,
                                VkPhysicalDevice physical_device,
                                QueueFamilies *families,
                                const char *extensions[],
                                uint32_t extensions_count);

VkPhysicalDevice magmatis_device_select(VkSurfaceKHR surface,
                                        VkInstance instance,
                                        const char *const *extensions,
                                        uint32_t extensions_count);

#ifndef USE_LONG_NAMES
#define device_create magmatis_device_create
#define device_select magmatis_device_select
#endif