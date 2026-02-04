#pragma once
#include <vk_mem_alloc.h>

VmaAllocator magmatis_vma_create(VkPhysicalDevice physical_device,
                                 VkDevice device, VkInstance instance);

#ifndef USE_LONG_NAMES
#define vma_create magmatis_vma_create
#endif