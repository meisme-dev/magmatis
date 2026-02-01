#include "queue.h"
#include <stdlib.h>

bool magmatis_queue_is_complete(QueueFamilies *queue) {
  return queue->graphics_family != UINT32_MAX &&
         queue->present_family != UINT32_MAX;
}

QueueFamilies magmatis_queue_families_find(VkPhysicalDevice device,
                                           VkSurfaceKHR surface) {
  QueueFamilies families = {UINT32_MAX, UINT32_MAX};
  uint32_t queueFamilyPropertiesCount = 0;

  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyPropertiesCount,
                                           NULL);

  VkQueueFamilyProperties *queueFamilyProperties =
      calloc(queueFamilyPropertiesCount, sizeof(VkQueueFamilyProperties));
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyPropertiesCount,
                                           queueFamilyProperties);

  for (uint32_t i = 0; i < queueFamilyPropertiesCount; i++) {
    if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      families.graphics_family = i;
    }

    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface,
                                         &families.present_family);
  }

  free(queueFamilyProperties);
  return families;
}

VkQueue magmatis_queue_get(VkDevice device, uint32_t family_index) {
  VkQueue queue;
  vkGetDeviceQueue(device, family_index, 0, &queue);
  return queue;
}
