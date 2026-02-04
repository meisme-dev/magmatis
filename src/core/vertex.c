#include "vertex.h"
#include "vulkan/vulkan_core.h"
#include <extra/colors.h>
#include <stdlib.h>
#include <string.h>

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
      calloc(2, sizeof(VkVertexInputAttributeDescription));

  attribute_description[0].binding = 0;
  attribute_description[0].location = 0;
  attribute_description[0].format = VK_FORMAT_R32G32B32_SFLOAT;
  attribute_description[0].offset = offsetof(Vertex, position);

  attribute_description[1].binding = 0;
  attribute_description[1].location = 1;
  attribute_description[1].format = VK_FORMAT_R32G32B32_SFLOAT;
  attribute_description[1].offset = offsetof(Vertex, color);

  return attribute_description;
}

VkBuffer magmatis_vertex_buffer_create(VmaAllocator allocator,
                                       VmaAllocation *allocation,
                                       Vertex *vertices, size_t size) {
  VkBufferCreateInfo buffer_info;
  memset(&buffer_info, 0, sizeof(buffer_info));
  buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_info.size = sizeof(Vertex) * size;
  buffer_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VkBuffer vertex_buffer;

  VmaAllocationCreateInfo allocation_create_info;
  memset(&allocation_create_info, 0, sizeof(allocation_create_info));
  allocation_create_info.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

  if (vmaCreateBuffer(allocator, &buffer_info, &allocation_create_info,
                      &vertex_buffer, allocation, NULL) != VK_SUCCESS) {
    fprintf(stderr, "%sFailed to create vertex buffer%s\n", RED, CLEAR);
    return NULL;
  }

  VmaAllocationInfo allocation_info;
  vmaGetAllocationInfo(allocator, *allocation, &allocation_info);

  void *mapped_data;
  vmaMapMemory(allocator, *allocation, &mapped_data);
  memcpy(mapped_data, vertices, sizeof(Vertex) * size);
  vmaUnmapMemory(allocator, *allocation);

  return vertex_buffer;
}
