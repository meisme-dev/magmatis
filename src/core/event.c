#include "event.h"
#include "command.h"
#include "render.h"
#include "swapchain.h"
#include <extra/colors.h>
#include <stdio.h>
#include <string.h>

int magmatis_event_loop_run(Magmatis *program) {
  vkWaitForFences(program->device, 1,
                  &program->in_flight_fences[program->current_frame], VK_TRUE,
                  -1);

  VkResult result =
      vkAcquireNextImageKHR(program->device, program->swapchain, UINT64_MAX,
                            program->image_semaphores[program->current_frame],
                            VK_NULL_HANDLE, &program->image_index);

  vkResetFences(program->device, 1,
                &program->in_flight_fences[program->current_frame]);

  VkSubmitInfo submit_info;
  memset(&submit_info, 0, sizeof(submit_info));

  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers =
      &program->command_buffers[program->current_frame];

  VkSemaphore wait_semaphores[] = {
      program->image_semaphores[program->current_frame]};
  VkPipelineStageFlags wait_stages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

  submit_info.waitSemaphoreCount = 1;
  submit_info.pWaitSemaphores = wait_semaphores;
  submit_info.pWaitDstStageMask = wait_stages;

  VkSemaphore signal_semaphores[] = {
      program->render_semaphores[program->current_frame]};
  submit_info.signalSemaphoreCount = 1;
  submit_info.pSignalSemaphores = signal_semaphores;

  vkResetCommandBuffer(program->command_buffers[program->current_frame], 0);
  command_buffer_begin(program->command_buffers[program->current_frame]);

  render(program->render_pass, program->command_buffers[program->current_frame],
         program->framebuffers[program->image_index], program->extent,
         program->pipeline, *program->pipeline_info->viewport_state->pViewports,
         *program->pipeline_info->viewport_state->pScissors,
         program->vertex_buffer, program->vertex_count);

  command_buffer_end(program->command_buffers[program->current_frame]);

  vkQueueWaitIdle(program->graphics_queue);
  if (vkQueueSubmit(program->graphics_queue, 1, &submit_info,
                    program->in_flight_fences[program->current_frame]) !=
      VK_SUCCESS) {
    fprintf(stderr, "%sFailed to submit draw command buffer%s\n", RED, CLEAR);
    return -1;
  }

  VkPresentInfoKHR present_info;
  memset(&present_info, 0, sizeof(present_info));

  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info.waitSemaphoreCount = 1;
  present_info.pWaitSemaphores = signal_semaphores;

  VkSwapchainKHR swapchains[] = {program->swapchain};
  present_info.swapchainCount = 1;
  present_info.pSwapchains = swapchains;

  present_info.pImageIndices = &program->image_index;

  if (vkQueuePresentKHR(program->present_queue, &present_info) != VK_SUCCESS) {
    fprintf(stderr, "%sFailed to present image%s\n", RED, CLEAR);
    return -1;
  }

  if (result == VK_ERROR_OUT_OF_DATE_KHR || program->framebuffer_resized) {
    program->framebuffer_resized = false;
    magmatis_swapchain_recreate(
        &program->swapchain, program->physical_device, program->device,
        &program->framebuffers, &program->image_views, program->surface,
        program->render_pass, program->width, program->height, &program->images,
        &program->image_count, &program->format, &program->extent);
    return 0;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    fprintf(stderr, "Failed to acquire next image\n");
    return -1;
  }

  program->current_frame = (program->current_frame + 1) % MAX_FRAMES_IN_FLIGHT;

  return 0;
}
