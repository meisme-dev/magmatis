#include "memory.h"
#include <extra/colors.h>
#include <stdio.h>
#include <string.h>

VmaAllocator magmatis_vma_create(VkPhysicalDevice physical_device,
                                 VkDevice device, VkInstance instance) {
  VmaAllocatorCreateInfo allocator_info;
  memset(&allocator_info, 0, sizeof(allocator_info));
  allocator_info.physicalDevice = physical_device;
  allocator_info.device = device;
  allocator_info.instance = instance;

  VmaAllocator allocator;
  if (vmaCreateAllocator(&allocator_info, &allocator) != VK_SUCCESS) {
    fprintf(stderr, "%sFailed to create VMA allocator%s\n", RED, CLEAR);
    return NULL;
  }

  return allocator;
}