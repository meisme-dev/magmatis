#include "pipeline.h"
#include "io/file.h"
#include "vulkan/vulkan_core.h"

#include <extra/colors.h>
#include <stdio.h>
#include <string.h>

VkShaderModule magmatis_shader_module_create(VkDevice device, char *code,
                                             size_t code_size) {
  VkShaderModuleCreateInfo shader_module_create_info;
  memset(&shader_module_create_info, 0, sizeof(shader_module_create_info));

  shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  shader_module_create_info.codeSize = code_size;
  shader_module_create_info.pCode = (uint32_t *)code;

  VkShaderModule shader_module;
  vkCreateShaderModule(device, &shader_module_create_info, NULL,
                       &shader_module);

  return shader_module;
}

VkRenderPass magmatis_render_pass_create(VkDevice device, VkFormat format) {
  VkAttachmentDescription *attachment_description =
      calloc(1, sizeof(VkAttachmentDescription));

  attachment_description->format = format;
  attachment_description->samples = VK_SAMPLE_COUNT_1_BIT;
  attachment_description->loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  attachment_description->storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  attachment_description->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  attachment_description->stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  attachment_description->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  attachment_description->finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference *color_attachment_reference =
      calloc(1, sizeof(VkAttachmentReference));

  color_attachment_reference->attachment = 0;
  color_attachment_reference->layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription *subpass_description =
      calloc(1, sizeof(VkSubpassDescription));

  subpass_description->pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass_description->colorAttachmentCount = 1;
  subpass_description->pColorAttachments = color_attachment_reference;

  VkRenderPassCreateInfo render_pass_create_info;
  memset(&render_pass_create_info, 0, sizeof(render_pass_create_info));

  render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_create_info.attachmentCount = 1;
  render_pass_create_info.pAttachments = attachment_description;
  render_pass_create_info.subpassCount = 1;
  render_pass_create_info.pSubpasses = subpass_description;

  VkSubpassDependency *subpass_dependency =
      calloc(1, sizeof(VkSubpassDependency));

  subpass_dependency->srcSubpass = VK_SUBPASS_EXTERNAL;
  subpass_dependency->dstSubpass = 0;
  subpass_dependency->srcStageMask =
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  subpass_dependency->dstStageMask =
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  subpass_dependency->srcAccessMask = 0;
  subpass_dependency->dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  render_pass_create_info.dependencyCount = 1;
  render_pass_create_info.pDependencies = subpass_dependency;

  VkRenderPass render_pass;
  if (vkCreateRenderPass(device, &render_pass_create_info, NULL,
                         &render_pass) != VK_SUCCESS) {
    fprintf(stderr, "%sFailed to create render pass%s\n", RED, CLEAR);
    return NULL;
  }

  return render_pass;
}

VkPipelineLayout magmatis_pipeline_layout_create(VkDevice device,
                                                 VkExtent2D extent,
                                                 PipelineInfo *pipeline_info) {

  size_t vertex_shader_size;
  size_t fragment_shader_size;

  char *vertex_shader =
      resource_file_read("shader.vert.spv", &vertex_shader_size);
  char *fragment_shader =
      resource_file_read("shader.frag.spv", &fragment_shader_size);

  VkShaderModule vertex_shader_module =
      magmatis_shader_module_create(device, vertex_shader, vertex_shader_size);
  VkShaderModule fragment_shader_module = magmatis_shader_module_create(
      device, fragment_shader, fragment_shader_size);

  VkPipelineShaderStageCreateInfo *vert_shader_create_info =
      calloc(1, sizeof(VkPipelineShaderStageCreateInfo));

  vert_shader_create_info->sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vert_shader_create_info->stage = VK_SHADER_STAGE_VERTEX_BIT;

  vert_shader_create_info->module = vertex_shader_module;
  vert_shader_create_info->pName = "main";

  VkPipelineShaderStageCreateInfo *frag_shader_create_info =
      calloc(1, sizeof(VkPipelineShaderStageCreateInfo));

  frag_shader_create_info->sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  frag_shader_create_info->stage = VK_SHADER_STAGE_FRAGMENT_BIT;

  frag_shader_create_info->module = fragment_shader_module;
  frag_shader_create_info->pName = "main";

  pipeline_info->shader_stages =
      calloc(2, sizeof(VkPipelineShaderStageCreateInfo));

  pipeline_info->shader_stages[0] = *vert_shader_create_info;
  pipeline_info->shader_stages[1] = *frag_shader_create_info;

  VkDynamicState *dynamic_states = calloc(2, sizeof(VkDynamicState));
  dynamic_states[0] = VK_DYNAMIC_STATE_VIEWPORT;
  dynamic_states[1] = VK_DYNAMIC_STATE_SCISSOR;

  pipeline_info->dynamic_state =
      calloc(1, sizeof(VkPipelineDynamicStateCreateInfo));
  pipeline_info->dynamic_state->sType =
      VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  pipeline_info->dynamic_state->dynamicStateCount = 2;
  pipeline_info->dynamic_state->pDynamicStates = dynamic_states;

  pipeline_info->vertex_input_state =
      calloc(1, sizeof(VkPipelineVertexInputStateCreateInfo));
  pipeline_info->vertex_input_state->sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  pipeline_info->vertex_input_state->vertexBindingDescriptionCount = 0;
  pipeline_info->vertex_input_state->pVertexBindingDescriptions = NULL;
  pipeline_info->vertex_input_state->vertexAttributeDescriptionCount = 0;
  pipeline_info->vertex_input_state->pVertexAttributeDescriptions = NULL;

  pipeline_info->input_assembly_state =
      calloc(1, sizeof(VkPipelineInputAssemblyStateCreateInfo));
  pipeline_info->input_assembly_state->sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  pipeline_info->input_assembly_state->topology =
      VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  pipeline_info->input_assembly_state->primitiveRestartEnable = VK_FALSE;

  VkViewport *viewport = calloc(1, sizeof(VkViewport));
  viewport->width = extent.width;
  viewport->height = extent.height;
  viewport->minDepth = 0.0f;
  viewport->maxDepth = 1.0f;

  VkRect2D *scissor = calloc(1, sizeof(VkRect2D));
  scissor->extent = extent;
  scissor->offset.x = 0;
  scissor->offset.y = 0;

  pipeline_info->viewport_state =
      calloc(1, sizeof(VkPipelineViewportStateCreateInfo));
  pipeline_info->viewport_state->sType =
      VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  pipeline_info->viewport_state->viewportCount = 1;
  pipeline_info->viewport_state->pViewports = viewport;
  pipeline_info->viewport_state->scissorCount = 1;
  pipeline_info->viewport_state->pScissors = scissor;

  pipeline_info->rasterization_state =
      calloc(1, sizeof(VkPipelineRasterizationStateCreateInfo));
  pipeline_info->rasterization_state->sType =
      VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  pipeline_info->rasterization_state->depthClampEnable = VK_FALSE;
  pipeline_info->rasterization_state->rasterizerDiscardEnable = VK_FALSE;
  pipeline_info->rasterization_state->polygonMode = VK_POLYGON_MODE_FILL;
  pipeline_info->rasterization_state->lineWidth = 1.0f;
  pipeline_info->rasterization_state->cullMode = VK_CULL_MODE_BACK_BIT;
  pipeline_info->rasterization_state->frontFace = VK_FRONT_FACE_CLOCKWISE;
  pipeline_info->rasterization_state->depthBiasEnable = VK_FALSE;
  pipeline_info->rasterization_state->depthBiasConstantFactor = 0.0f;
  pipeline_info->rasterization_state->depthBiasClamp = 0.0f;
  pipeline_info->rasterization_state->depthBiasSlopeFactor = 0.0f;

  pipeline_info->multisample_state =
      calloc(1, sizeof(VkPipelineMultisampleStateCreateInfo));
  pipeline_info->multisample_state->sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  pipeline_info->multisample_state->rasterizationSamples =
      VK_SAMPLE_COUNT_1_BIT;
  pipeline_info->multisample_state->sampleShadingEnable = VK_FALSE;
  pipeline_info->multisample_state->minSampleShading = 1.0f;
  pipeline_info->multisample_state->pSampleMask = NULL;
  pipeline_info->multisample_state->alphaToCoverageEnable = VK_FALSE;
  pipeline_info->multisample_state->alphaToOneEnable = VK_FALSE;

  pipeline_info->depth_stencil_state =
      calloc(1, sizeof(VkPipelineDepthStencilStateCreateInfo));
  pipeline_info->depth_stencil_state->sType =
      VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  pipeline_info->depth_stencil_state->depthTestEnable = VK_TRUE;
  pipeline_info->depth_stencil_state->depthWriteEnable = VK_TRUE;
  pipeline_info->depth_stencil_state->depthCompareOp = VK_COMPARE_OP_LESS;
  pipeline_info->depth_stencil_state->depthBoundsTestEnable = VK_FALSE;
  pipeline_info->depth_stencil_state->minDepthBounds = 0.0f;
  pipeline_info->depth_stencil_state->maxDepthBounds = 1.0f;
  pipeline_info->depth_stencil_state->stencilTestEnable = VK_FALSE;

  VkPipelineColorBlendAttachmentState *color_blend_attachment_state =
      calloc(1, sizeof(VkPipelineColorBlendAttachmentState));

  color_blend_attachment_state->blendEnable = VK_FALSE;
  color_blend_attachment_state->srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  color_blend_attachment_state->dstColorBlendFactor =
      VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  color_blend_attachment_state->colorBlendOp = VK_BLEND_OP_ADD;
  color_blend_attachment_state->srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  color_blend_attachment_state->dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  color_blend_attachment_state->alphaBlendOp = VK_BLEND_OP_ADD;
  color_blend_attachment_state->colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

  pipeline_info->color_blend_state_create_info =
      calloc(1, sizeof(VkPipelineColorBlendStateCreateInfo));
  pipeline_info->color_blend_state_create_info->sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  pipeline_info->color_blend_state_create_info->logicOpEnable = VK_FALSE;
  pipeline_info->color_blend_state_create_info->logicOp = VK_LOGIC_OP_COPY;
  pipeline_info->color_blend_state_create_info->attachmentCount = 1;
  pipeline_info->color_blend_state_create_info->pAttachments =
      color_blend_attachment_state;
  pipeline_info->color_blend_state_create_info->blendConstants[0] = 0.0f;
  pipeline_info->color_blend_state_create_info->blendConstants[1] = 0.0f;
  pipeline_info->color_blend_state_create_info->blendConstants[2] = 0.0f;
  pipeline_info->color_blend_state_create_info->blendConstants[3] = 0.0f;

  VkPipelineLayoutCreateInfo pipeline_layout_create_info;
  memset(&pipeline_layout_create_info, 0, sizeof(pipeline_layout_create_info));

  pipeline_layout_create_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout_create_info.setLayoutCount = 0;
  pipeline_layout_create_info.pSetLayouts = NULL;
  pipeline_layout_create_info.pushConstantRangeCount = 0;
  pipeline_layout_create_info.pPushConstantRanges = NULL;

  VkPipelineLayout pipeline_layout;
  if (vkCreatePipelineLayout(device, &pipeline_layout_create_info, NULL,
                             &pipeline_layout) != VK_SUCCESS) {
    fprintf(stderr, "%sFailed to create pipeline layout%s\n", RED, CLEAR);
    return NULL;
  }

  return pipeline_layout;
}

VkPipeline magmatis_pipeline_create(VkDevice device, VkRenderPass render_pass,
                                    VkPipelineLayout pipeline_layout,
                                    PipelineInfo *pipeline_info) {
  VkGraphicsPipelineCreateInfo pipeline_create_info;
  memset(&pipeline_create_info, 0, sizeof(pipeline_create_info));

  pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipeline_create_info.stageCount = 2;
  pipeline_create_info.pStages = pipeline_info->shader_stages;
  pipeline_create_info.pVertexInputState = pipeline_info->vertex_input_state;
  pipeline_create_info.pInputAssemblyState =
      pipeline_info->input_assembly_state;
  pipeline_create_info.pViewportState = pipeline_info->viewport_state;
  pipeline_create_info.pRasterizationState = pipeline_info->rasterization_state;
  pipeline_create_info.pMultisampleState = pipeline_info->multisample_state;
  pipeline_create_info.pDepthStencilState = pipeline_info->depth_stencil_state;
  pipeline_create_info.pColorBlendState =
      pipeline_info->color_blend_state_create_info;
  pipeline_create_info.pDynamicState = pipeline_info->dynamic_state;
  pipeline_create_info.layout = pipeline_layout;
  pipeline_create_info.renderPass = render_pass;
  pipeline_create_info.subpass = 0;
  pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
  pipeline_create_info.basePipelineIndex = -1;

  VkPipeline pipeline;
  if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1,
                                &pipeline_create_info, NULL,
                                &pipeline) != VK_SUCCESS) {
    fprintf(stderr, "%sFailed to create graphics pipeline%s\n", RED, CLEAR);
    return NULL;
  }

  vkDestroyShaderModule(device, pipeline_info->shader_stages[0].module, NULL);
  vkDestroyShaderModule(device, pipeline_info->shader_stages[1].module, NULL);

  return pipeline;
}