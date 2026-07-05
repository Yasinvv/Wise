module;

#include <cassert>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

export module imageViews;
import context;
import extra;

namespace WisE {

export class ImageViews {

private:
public:
  void createImageViews(VK_CTX& ctx) {
    assert(ctx.swapChainImageViews.empty());

    ctx.swapChainImageViews.reserve(ctx.swapChainImages.size());
    for (auto& image : ctx.swapChainImages) {
      ctx.swapChainImageViews.emplace_back(
          createImageView(image, ctx.swapChainSurfaceFormat.format,
                          vk::ImageAspectFlagBits::eColor, ctx.device));
    }
  }
};
} // namespace WisE
