
module;

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

export module commandBuffer;
import context;
import extra;

namespace WisE {
export class CommandBuffer {
private:
public:
  vk::raii::CommandBuffer beginSingleTimeCommands(VK_CTX& ctx) {
    vk::CommandBufferAllocateInfo allocInfo{
        .commandPool = ctx.commandPool,
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1};
    vk::raii::CommandBuffer commandBuffer =
        std::move(vk::raii::CommandBuffers(ctx.device, allocInfo).front());

    vk::CommandBufferBeginInfo beginInfo{
        .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit};
    commandBuffer.begin(beginInfo);

    return commandBuffer;
  }
  void endSingleTimeCommands(vk::raii::CommandBuffer&& commandBuffer,
                             VK_CTX& ctx) {
    commandBuffer.end();

    vk::SubmitInfo submitInfo{.commandBufferCount = 1,
                              .pCommandBuffers = &*commandBuffer};
    ctx.queue.submit(submitInfo, nullptr);
    ctx.queue.waitIdle();
  }
};

} // namespace WisE
