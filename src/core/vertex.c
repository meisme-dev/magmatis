#include "vertex.h"
#include "vulkan/vulkan_core.h"
#include <extra/colors.h>
#include <stdlib.h>

VkVertexInputBindingDescription *magmatis_vertex_binding_description_get(void) {
  VkVertexInputBindingDescription *binding_description =
      calloc(1, sizeof(VkVertexInputBindingDescription));
  binding_description->binding = 0;
  binding_description->stride = sizeof(Vertex);
  binding_description->inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  return binding_description;
}

VkVertexInputAttributeDescription *
magmatis_vertex_attribute_description_get(void) {
  VkVertexInputAttributeDescription *attribute_description =
      calloc(3, sizeof(VkVertexInputAttributeDescription));

  attribute_description[0].binding = 0;
  attribute_description[0].location = 0;
  attribute_description[0].format = VK_FORMAT_R32G32B32_SFLOAT;
  attribute_description[0].offset = offsetof(Vertex, position);

  attribute_description[1].binding = 0;
  attribute_description[1].location = 1;
  attribute_description[1].format = VK_FORMAT_R32G32B32_SFLOAT;
  attribute_description[1].offset = offsetof(Vertex, color);

  attribute_description[2].binding = 0;
  attribute_description[2].location = 2;
  attribute_description[2].format = VK_FORMAT_R32G32_SFLOAT;
  attribute_description[2].offset = offsetof(Vertex, tex_coord);

  return attribute_description;
}
