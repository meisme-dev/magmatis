#pragma once
#include <vulkan/vulkan.h>

VkInstance magmatis_instance_create(const char *const *layers,
                                    uint32_t layers_count,
                                    char *application_name, char *engine_name,
                                    uint32_t application_version,
                                    uint32_t engine_version,
                                    uint32_t api_version);

#ifndef USE_LONG_NAMES
#define instance_create magmatis_instance_create
#endif