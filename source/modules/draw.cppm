module;

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

export module draw;
import context;
import swapchain;
import camera;
import commandBuffer;

namespace WisE {
export class Draw {
private:
public:
  void drawFrame(VK_CTX& ctx, float& deltaTime, Configs& configs,
                 Swapchain& swap, CommandBuffer& commandBuffer,
                 SDL_Window* sdlwindow, SDL_Event& event, Camera& camera,
                 InfiniteGrid& m_infiniteGrid, Model_CTX& model) {
    // Note: inFlightFences, presentCompleteSemaphores, and commandBuffers are
    // indexed by frameIndex,
    //       while renderFinishedSemaphores is indexed by imageIndex
    auto fenceResult = ctx.device.waitForFences(
        {ctx.inFlightFences[ctx.frameIndex]}, vk::True, UINT64_MAX);
    if (fenceResult != vk::Result::eSuccess) {
      throw std::runtime_error("failed to wait for fence!");
    }

    auto [result, imageIndex] = ctx.swapChain.acquireNextImage(
        UINT64_MAX, *ctx.presentCompleteSemaphores[ctx.frameIndex], nullptr);

    // Due to VULKAN_HPP_HANDLE_ERROR_OUT_OF_DATE_AS_SUCCESS being defined,
    // eErrorOutOfDateKHR can be checked as a result here and does not need to
    // be caught by an exception.
    if (result == vk::Result::eErrorOutOfDateKHR) {
      swap.recreateSwapChain(sdlwindow, event);
      return;
    }
    // On other success codes than eSuccess and eSuboptimalKHR we just throw
    // an exception. On any error code, aquireNextImage already threw an
    // exception.
    if (result != vk::Result::eSuccess &&
        result != vk::Result::eSuboptimalKHR) {
      assert(result == vk::Result::eTimeout || result == vk::Result::eNotReady);
      throw std::runtime_error("failed to acquire swap chain image!");
    }
    commandBuffer.updateUniformBuffer(ctx.frameIndex, ctx, camera);

    // Only reset the fence if we are submitting work
    ctx.device.resetFences(*ctx.inFlightFences[ctx.frameIndex]);

    ctx.commandBuffers[ctx.frameIndex].reset();
    commandBuffer.recordCommandBuffer(imageIndex, deltaTime, configs, ctx,
                                      m_infiniteGrid, model);

    vk::PipelineStageFlags waitDestinationStageMask(
        vk::PipelineStageFlagBits::eColorAttachmentOutput);
    const vk::SubmitInfo submitInfo{
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &*ctx.presentCompleteSemaphores[ctx.frameIndex],
        .pWaitDstStageMask = &waitDestinationStageMask,
        .commandBufferCount = 1,
        .pCommandBuffers = &*ctx.commandBuffers[ctx.frameIndex],
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &*ctx.renderFinishedSemaphores[imageIndex]};
    ctx.queue.submit(submitInfo, *ctx.inFlightFences[ctx.frameIndex]);

    const vk::PresentInfoKHR presentInfoKHR{
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &*ctx.renderFinishedSemaphores[imageIndex],
        .swapchainCount = 1,
        .pSwapchains = &*ctx.swapChain,
        .pImageIndices = &imageIndex};
    result = ctx.queue.presentKHR(presentInfoKHR);
    // Due to VULKAN_HPP_HANDLE_ERROR_OUT_OF_DATE_AS_SUCCESS being defined,
    // eErrorOutOfDateKHR can be checked as a result here and does not need to
    // be caught by an exception.
    if ((result == vk::Result::eSuboptimalKHR) ||
        (result == vk::Result::eErrorOutOfDateKHR) || ctx.framebufferResized) {
      ctx.framebufferResized = false;
      swap.recreateSwapChain(sdlwindow, event);
    } else {
      // There are no other success codes than eSuccess; on any error code,
      // presentKHR already threw an exception.
      assert(result == vk::Result::eSuccess);
    }
    ctx.frameIndex = (ctx.frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
  }
};

} // namespace WisE
