#pragma once
#include <cglm/cglm.h>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

typedef struct Vertex {
  vec3 position;
  vec3 color;
} Vertex;

typedef struct VertexBuffer {
  Vertex *vertices;
  size_t size;
} VertexBuffer;

VkVertexInputBindingDescription *magmatis_vertex_binding_description_get(void);
VkVertexInputAttributeDescription *
magmatis_vertex_attribute_description_get(void);

#ifndef USE_LONG_NAMES
#define vertex_binding_description_get magmatis_vertex_binding_description_get
#define vertex_attribute_description_get                                       \
  magmatis_vertex_attribute_description_get
#endif