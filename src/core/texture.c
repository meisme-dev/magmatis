#include "command.h"
#include "vulkan/vulkan_core.h"
#define STB_IMAGE_IMPLEMENTATION

#include "buffer.h"
#include "texture.h"
#include <extra/colors.h>
#include <io/file.h>
#include <vulkan/vulkan.h>

VkImage magmatis_texture_create(VmaAllocator allocator,
                                VmaAllocation *image_allocation, VkQueue queue,
                                VkCommandPool command_pool, VkDevice device,
                                const char *path) {
  int tex_width, tex_height, tex_channels;

  char *file_path = magmatis_resource_file_path_get(path);

  stbi_uc *pixels = stbi_load(file_path, &tex_width, &tex_height, &tex_channels,
                              STBI_rgb_alpha);

  VkDeviceSize image_size = tex_width * tex_height * 4;

  if (!pixels) {
    fprintf(stderr, "%sFailed to load texture image %s%s\n", RED, file_path,
            CLEAR);
    return NULL;
  }

  VmaAllocation buffer_allocation;

  VkBuffer buffer = magmatis_buffer_create(
      allocator, &buffer_allocation, image_size,
      VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE,
      VMA_MEMORY_USAGE_CPU_TO_GPU,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
      0);

  void *data;
  vmaMapMemory(allocator, buffer_allocation, &data);
  memcpy(data, pixels, image_size);
  vmaUnmapMemory(allocator, buffer_allocation);

  stbi_image_free(pixels);

  VkImage image = magmatis_texture_image_create(allocator, image_allocation,
                                                device, tex_width, tex_height);

  magmatis_image_layout_transition(image, device, queue, command_pool,
                                   VK_IMAGE_LAYOUT_UNDEFINED,
                                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

  magmatis_buffer_to_image_copy(device, queue, command_pool, buffer, image,
                                tex_width, tex_height);

  magmatis_image_layout_transition(image, device, queue, command_pool,
                                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                   VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  vmaFreeMemory(allocator, buffer_allocation);
  vkDestroyBuffer(device, buffer, NULL);

  return image;
}

VkImage magmatis_texture_image_create(VmaAllocator allocator,
                                      VmaAllocation *allocation,
                                      VkDevice device, uint32_t tex_width,
                                      uint32_t tex_height) {
  VkImageCreateInfo image_create_info = {0};
  image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  image_create_info.imageType = VK_IMAGE_TYPE_2D;
  image_create_info.extent.width = tex_width;
  image_create_info.extent.height = tex_height;
  image_create_info.extent.depth = 1;
  image_create_info.mipLevels = 1;
  image_create_info.arrayLayers = 1;
  image_create_info.format = VK_FORMAT_R8G8B8A8_SRGB;
  image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
  image_create_info.usage =
      VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
  image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
  image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VkImage image;
  if (vkCreateImage(device, &image_create_info, NULL, &image) != VK_SUCCESS) {
    fprintf(stderr, "%sFailed to create image%s\n", RED, CLEAR);
    return NULL;
  }

  VmaAllocationCreateInfo alloc_info = {0};
  alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;

  if (vmaAllocateMemoryForImage(allocator, image, &alloc_info, allocation,
                                NULL) != VK_SUCCESS) {
    fprintf(stderr, "%sFailed to allocate memory for image%s\n", RED, CLEAR);
    vkDestroyImage(device, image, NULL);
    return NULL;
  }

  if (vmaBindImageMemory(allocator, *allocation, image) != VK_SUCCESS) {
    fprintf(stderr, "%sFailed to bind image memory%s\n", RED, CLEAR);
    vkDestroyImage(device, image, NULL);
    vmaFreeMemory(allocator, *allocation);
    return NULL;
  }

  return image;
}

void magmatis_image_layout_transition(VkImage image, VkDevice device,
                                      VkQueue graphics_queue,
                                      VkCommandPool command_pool,
                                      VkImageLayout old_layout,
                                      VkImageLayout new_layout) {
  VkCommandBuffer *command_buffer =
      magmatis_command_buffer_create(device, command_pool);
  magmatis_command_buffer_begin(*command_buffer);

  VkImageMemoryBarrier image_memory_barrier = {};
  image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  image_memory_barrier.oldLayout = old_layout;
  image_memory_barrier.newLayout = new_layout;
  image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  image_memory_barrier.image = image;
  image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  image_memory_barrier.subresourceRange.baseMipLevel = 0;
  image_memory_barrier.subresourceRange.levelCount = 1;
  image_memory_barrier.subresourceRange.baseArrayLayer = 0;
  image_memory_barrier.subresourceRange.layerCount = 1;

  VkPipelineStageFlags source_stage;
  VkPipelineStageFlags destination_stage;

  if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
      new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {

    image_memory_barrier.srcAccessMask = 0;
    image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;

  } else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
             new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {

    image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

  } else {
    fprintf(stderr, "Unsupported layout transition\n");
    return;
  }

  vkCmdPipelineBarrier(*command_buffer, source_stage, destination_stage, 0, 0,
                       NULL, 0, NULL, 1, &image_memory_barrier);

  magmatis_command_buffer_end(*command_buffer);

  VkSubmitInfo submit_info;
  memset(&submit_info, 0, sizeof(submit_info));
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = command_buffer;

  if (vkQueueSubmit(graphics_queue, 1, &submit_info, VK_NULL_HANDLE) !=
      VK_SUCCESS) {
    fprintf(stderr, "%sFailed to submit queue%s\n", RED, CLEAR);
    return;
  }

  vkQueueWaitIdle(graphics_queue);
  vkFreeCommandBuffers(device, command_pool, 1, command_buffer);
}

void magmatis_buffer_to_image_copy(VkDevice device, VkQueue queue,
                                   VkCommandPool command_pool, VkBuffer buffer,
                                   VkImage image, uint32_t width,
                                   uint32_t height) {
  VkCommandBuffer *command_buffer =
      magmatis_command_buffer_create(device, command_pool);
  magmatis_command_buffer_begin(*command_buffer);

  VkBufferImageCopy region = {};
  region.bufferOffset = 0;
  region.bufferRowLength = 0;
  region.bufferImageHeight = 0;
  region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel = 0;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount = 1;
  region.imageOffset = (VkOffset3D){0, 0, 0};
  region.imageExtent = (VkExtent3D){width, height, 1};

  vkCmdCopyBufferToImage(*command_buffer, buffer, image,
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

  magmatis_command_buffer_end(*command_buffer);

  VkSubmitInfo submit_info;
  memset(&submit_info, 0, sizeof(submit_info));
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = command_buffer;

  if (vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE) != VK_SUCCESS) {
    fprintf(stderr, "%sFailed to submit queue%s\n", RED, CLEAR);
    return;
  }

  vkQueueWaitIdle(queue);
  vkFreeCommandBuffers(device, command_pool, 1, command_buffer);
}

VkSampler magmatis_texture_sampler_create(VkPhysicalDevice physical_device,
                                          VkDevice device) {
  VkSamplerCreateInfo sampler_info = {};
  sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  sampler_info.magFilter = VK_FILTER_LINEAR;
  sampler_info.minFilter = VK_FILTER_LINEAR;
  sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_info.anisotropyEnable = VK_TRUE;

  VkPhysicalDeviceProperties properties;
  vkGetPhysicalDeviceProperties(physical_device, &properties);
  sampler_info.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
  sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  sampler_info.unnormalizedCoordinates = VK_FALSE;
  sampler_info.compareEnable = VK_FALSE;
  sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
  sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  sampler_info.mipLodBias = 0.0f;
  sampler_info.minLod = 0.0f;
  sampler_info.maxLod = 0.0f;

  VkSampler sampler;
  if (vkCreateSampler(device, &sampler_info, NULL, &sampler) != VK_SUCCESS) {
    fprintf(stderr, "%sFailed to create texture sampler%s\n", RED, CLEAR);
    return NULL;
  }

  return sampler;
}