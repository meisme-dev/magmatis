#include "device.h"
#include "queue.h"
#include "swapchain.h"
#include <extra/colors.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool magmatis_device_extensions_check(VkPhysicalDevice device,
                                             const char *extension) {
  uint32_t extension_count = 0;
  vkEnumerateDeviceExtensionProperties(device, NULL, &extension_count, NULL);

  VkExtensionProperties *extensions =
      calloc(extension_count, sizeof(VkExtensionProperties));
  vkEnumerateDeviceExtensionProperties(device, NULL, &extension_count,
                                       extensions);

  bool result = false;
  for (uint32_t i = 0; i < extension_count; i++) {
    if (strcmp(extensions[i].extensionName, extension) == 0) {
      result = true;
    }
  }

  free(extensions);
  return result;
}

VkPhysicalDevice magmatis_device_select(VkSurfaceKHR surface,
                                        VkInstance instance,
                                        const char *const *extensions,
                                        uint32_t extensions_count) {

  uint32_t device_count = 0;

  vkEnumeratePhysicalDevices(instance, &device_count, NULL);

  VkPhysicalDevice *devices = calloc(device_count, sizeof(VkPhysicalDevice));
  vkEnumeratePhysicalDevices(instance, &device_count, devices);

  if (device_count == 0) {
    fprintf(stderr, "%sNo devices found%s\n", RED, CLEAR);
    return NULL;
  }

  int highest_priority = 0;
  VkPhysicalDevice selected_device = NULL;

  for (uint32_t i = 0; i < device_count; i++) {
    int current_priority = 0;
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(devices[i], &properties);

    switch (properties.deviceType) {
      case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
        current_priority = 4;
      case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        current_priority = 3;
      case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
        current_priority = 2;
      default:
        current_priority = 1;
    }

    current_priority *= properties.limits.maxImageDimension2D;

    if (current_priority > highest_priority) {
      highest_priority = current_priority;

      for (uint32_t j = 0; j < extensions_count; j++) {
        if (!magmatis_device_extensions_check(devices[i], extensions[j])) {
          printf("Device %s does not support extension %s\n",
                 properties.deviceName, extensions[j]);
          break;
        }
      }

      SwapchainInfo *info = swapchain_info_query(devices[i], surface);

      if (info == NULL) {
        printf("Failed to query swapchain info for device %s\n",
               properties.deviceName);
        continue;
      }

      if (info->formats_count == 0 || info->present_modes_count == 0) {
        printf("Device %s does not support swapchain\n", properties.deviceName);
        free(info);
        continue;
      }

      free(info->formats);
      free(info->present_modes);
      free(info);

      selected_device = devices[i];
      printf("Selected physical device: %s\n", properties.deviceName);
    }
  }

  free(devices);
  return selected_device;
}

VkDevice magmatis_device_create(VkSurfaceKHR surface,
                                VkPhysicalDevice physical_device,
                                QueueFamilies *families,
                                const char *extensions[],
                                uint32_t extensions_count) {
  *families = queue_families_find(physical_device, surface);
  VkDeviceQueueCreateInfo queue_create_infos[MAGMATIS_QUEUE_FAMILY_COUNT];

  uint32_t indices[MAGMATIS_QUEUE_FAMILY_COUNT];
  uint32_t unique_queue_count = MAGMATIS_QUEUE_FAMILY_COUNT;

  indices[0] = families->graphics_family;
  indices[1] = families->present_family;

  // TODO: Count unique indices for any array size
  if (indices[0] == indices[1]) {
    unique_queue_count--;
  }

  float queue_priority = 1.0f;

  memset(queue_create_infos, 0, sizeof(queue_create_infos));

  for (size_t i = 0; i < unique_queue_count; i++) {
    queue_create_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_infos[i].queueFamilyIndex = indices[i];
    queue_create_infos[i].queueCount = 1;
    queue_create_infos[i].pQueuePriorities = &queue_priority;
  }

  VkPhysicalDeviceFeatures device_features;
  memset(&device_features, 0, sizeof(device_features));
  vkGetPhysicalDeviceFeatures(physical_device, &device_features);

  VkDeviceCreateInfo device_create_info;
  memset(&device_create_info, 0, sizeof(device_create_info));

  device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  device_create_info.pQueueCreateInfos = queue_create_infos;
  device_create_info.queueCreateInfoCount = unique_queue_count;
  device_create_info.pEnabledFeatures = &device_features;

  device_create_info.enabledExtensionCount = extensions_count;
  device_create_info.ppEnabledExtensionNames = extensions;

  VkDevice device;

  if (vkCreateDevice(physical_device, &device_create_info, NULL, &device) !=
      VK_SUCCESS) {
    fprintf(stderr, "%sFailed to create device%s", RED, CLEAR);
    return NULL;
  }

  return device;
}
