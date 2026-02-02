#include "sync.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void magmatis_sync_create(VkDevice device, VkSemaphore *image_semaphores,
                          VkSemaphore *render_semaphores,
                          VkFence *in_flight_fences, uint32_t count) {
  VkSemaphoreCreateInfo semaphore_create_info;
  memset(&semaphore_create_info, 0, sizeof(semaphore_create_info));
  semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fence_create_info;
  memset(&fence_create_info, 0, sizeof(fence_create_info));
  fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_create_info.pNext = NULL;
  fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (uint32_t i = 0; i < count; i++) {
    VkResult result =
        vkCreateSemaphore(device, &semaphore_create_info, NULL,
                          &image_semaphores[i]) ||
        vkCreateSemaphore(device, &semaphore_create_info, NULL,
                          &render_semaphores[i]) ||
        vkCreateFence(device, &fence_create_info, NULL, &in_flight_fences[i]);
    if (result != VK_SUCCESS) {
      fprintf(stderr, "Failed to create semaphore!\n");
      return;
    }
  }
}
