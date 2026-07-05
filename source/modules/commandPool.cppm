
module;

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

export module commandPool;
import context;

namespace WisE {
export class CommandPool {
private:
public:
  void createCommandPool(VK_CTX& ctx) {
    vk::CommandPoolCreateInfo poolInfo{
        .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = ctx.queueIndex};
    ctx.commandPool = vk::raii::CommandPool(ctx.device, poolInfo);
  }
};

} // namespace WisE
