#include "instance.h"
#include "window.h"
#include <extra/colors.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

VkInstance magmatis_instance_create(const char *const *layers,
                                    uint32_t layers_count,
                                    char *application_name, char *engine_name,
                                    uint32_t application_version,
                                    uint32_t engine_version,
                                    uint32_t api_version) {

  VkApplicationInfo application_info;
  memset(&application_info, 0, sizeof(application_info));
  application_info.pApplicationName = application_name;
  application_info.applicationVersion = application_version;
  application_info.pEngineName = engine_name;
  application_info.engineVersion = engine_version;
  application_info.apiVersion = api_version;

  char *found_layers = calloc(layers_count, sizeof(char));

  uint32_t layer_count = 0;

  uint32_t layer_prop_count = 0;
  vkEnumerateInstanceLayerProperties(&layer_prop_count, NULL);

  VkLayerProperties layer_properties[layer_prop_count];
  vkEnumerateInstanceLayerProperties(&layer_prop_count, layer_properties);

  printf("%sAvailabe layers:\n", CLEAR);
  for (uint32_t i = 0; i < layer_prop_count - 1; i++) {
    if (i != layer_prop_count - 2) {
      printf("%s├── %s%s\n", BLUE, CLEAR, layer_properties[i].layerName);
    } else {
      printf("%s└── %s%s\n\n", BLUE, CLEAR, layer_properties[i].layerName);
    }

    for (uint32_t j = 0; j < layers_count; j++) {
      if (strcmp(layer_properties[i].layerName, layers[j]) == 0) {
        found_layers[j] = 1;
      }
    }
  }

  for (uint32_t j = 0; j < layers_count; j++) {
    if (found_layers[j] == 0) {
      fprintf(stderr, "%sLayer %s not found%s\n", RED, layers[j], CLEAR);
      return NULL;
    }
  }

  layer_count = layers_count;

  uint32_t required_extension_count = 0;
  const char **required_extensions;

  required_extensions = window_extensions_get(&required_extension_count);

  VkInstanceCreateInfo instance_create_info;
  memset(&instance_create_info, 0, sizeof(instance_create_info));

  instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instance_create_info.pApplicationInfo = &application_info;
  instance_create_info.ppEnabledExtensionNames = required_extensions;
  instance_create_info.enabledExtensionCount = required_extension_count;
  instance_create_info.enabledLayerCount = layer_count;
  instance_create_info.ppEnabledLayerNames = layers;

  VkInstance instance;
  if (vkCreateInstance(&instance_create_info, NULL, &instance) != VK_SUCCESS) {
    fprintf(stderr, "%sFailed to create instance%s\n", RED, CLEAR);
    return NULL;
  };

  return instance;
}
