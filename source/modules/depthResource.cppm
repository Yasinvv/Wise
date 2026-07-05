module;

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

export module depthResource;
import context;
import extra;

namespace WisE {
export class DepthResource {
private:
public:
  void createDepthResources(VK_CTX& ctx) {
    vk::Format depthFormat = findDepthFormat(ctx.physicalDevice);

    std::tie(ctx.depthImage, ctx.depthImageMemory) =
        createImage(ctx.swapChainExtent.width, ctx.swapChainExtent.height,
                    depthFormat, vk::ImageTiling::eOptimal,
                    vk::ImageUsageFlagBits::eDepthStencilAttachment,
                    vk::MemoryPropertyFlagBits::eDeviceLocal, ctx.device,
                    ctx.physicalDevice);
    ctx.depthImageView =
        createImageView(ctx.depthImage, depthFormat,
                        vk::ImageAspectFlagBits::eDepth, ctx.device);
  }
};

} // namespace WisE
