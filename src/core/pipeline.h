#pragma once

#include <vulkan/vulkan.h>

typedef struct PipelineInfo {
  VkPipelineShaderStageCreateInfo *shader_stages;
  VkPipelineVertexInputStateCreateInfo *vertex_input_state;
  VkPipelineInputAssemblyStateCreateInfo *input_assembly_state;
  VkPipelineViewportStateCreateInfo *viewport_state;
  VkPipelineRasterizationStateCreateInfo *rasterization_state;
  VkPipelineMultisampleStateCreateInfo *multisample_state;
  VkPipelineDepthStencilStateCreateInfo *depth_stencil_state;
  VkPipelineColorBlendStateCreateInfo *color_blend_state_create_info;
  VkPipelineDynamicStateCreateInfo *dynamic_state;
} PipelineInfo;

VkShaderModule magmatis_shader_module_create(VkDevice device, char *code,
                                             size_t code_size);

VkPipelineLayout
magmatis_pipeline_layout_create(VkDevice device, VkExtent2D extent,
                                PipelineInfo *pipeline_info,
                                VkDescriptorSetLayout descriptor_set_layouts,
                                uint32_t descriptor_set_layouts_count);

VkRenderPass magmatis_render_pass_create(VkDevice device, VkFormat format);

VkPipeline magmatis_pipeline_create(VkDevice device, VkRenderPass render_pass,
                                    VkPipelineLayout pipeline_layout,
                                    PipelineInfo *pipeline_info);

#ifndef USE_LONG_NAMES
#define pipeline_layout_create magmatis_pipeline_layout_create
#define shader_module_create magmatis_shader_module_create
#define render_pass_create magmatis_render_pass_create
#define pipeline_create magmatis_pipeline_create
#endif