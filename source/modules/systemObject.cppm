module;

#include <cassert>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

export module systemObject;
import context;

namespace WisE {
export class SystemObject {
private:
public:
  void createSyncObjects(VK_CTX& ctx) {
    assert(ctx.presentCompleteSemaphores.empty() &&
           ctx.renderFinishedSemaphores.empty() && ctx.inFlightFences.empty());

    for (size_t i = 0; i < ctx.swapChainImages.size(); i++) {
      ctx.renderFinishedSemaphores.emplace_back(ctx.device,
                                                vk::SemaphoreCreateInfo());
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      ctx.presentCompleteSemaphores.emplace_back(ctx.device,
                                                 vk::SemaphoreCreateInfo());
      ctx.inFlightFences.emplace_back(
          ctx.device,
          vk::FenceCreateInfo{.flags = vk::FenceCreateFlagBits::eSignaled});
    }
  }
};
} // namespace WisE
