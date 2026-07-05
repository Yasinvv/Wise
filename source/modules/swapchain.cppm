module;

#include <SDL3/SDL_vulkan.h>
#include <algorithm>
#include <limits>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

export module swapchain;
import context;
import window;

namespace WisE {

export class Swapchain {
private:
public:
  void createSwapChain(VK_CTX& ctx, Window& window) {
    vk::SurfaceCapabilitiesKHR surfaceCapabilities =
        ctx.physicalDevice.getSurfaceCapabilitiesKHR(*ctx.surface);
    ctx.swapChainExtent = chooseSwapExtent(surfaceCapabilities, window.window);
    uint32_t minImageCount = chooseSwapMinImageCount(surfaceCapabilities);

    std::vector<vk::SurfaceFormatKHR> availableFormats =
        ctx.physicalDevice.getSurfaceFormatsKHR(*ctx.surface);
    ctx.swapChainSurfaceFormat = chooseSwapSurfaceFormat(availableFormats);

    std::vector<vk::PresentModeKHR> availablePresentModes =
        ctx.physicalDevice.getSurfacePresentModesKHR(*ctx.surface);
    vk::PresentModeKHR presentMode =
        chooseSwapPresentMode(availablePresentModes);

    vk::SwapchainCreateInfoKHR swapChainCreateInfo{
        .surface = *ctx.surface,
        .minImageCount = minImageCount,
        .imageFormat = ctx.swapChainSurfaceFormat.format,
        .imageColorSpace = ctx.swapChainSurfaceFormat.colorSpace,
        .imageExtent = ctx.swapChainExtent,
        .imageArrayLayers = 1,
        .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
        .imageSharingMode = vk::SharingMode::eExclusive,
        .preTransform = surfaceCapabilities.currentTransform,
        .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
        .presentMode = presentMode,
        .clipped = true};

    ctx.swapChain = vk::raii::SwapchainKHR(ctx.device, swapChainCreateInfo);
    ctx.swapChainImages = ctx.swapChain.getImages();
  }

  static uint32_t chooseSwapMinImageCount(
      vk::SurfaceCapabilitiesKHR const& surfaceCapabilities) {
    auto minImageCount = std::max(3u, surfaceCapabilities.minImageCount);
    if ((0 < surfaceCapabilities.maxImageCount) &&
        (surfaceCapabilities.maxImageCount < minImageCount)) {
      minImageCount = surfaceCapabilities.maxImageCount;
    }
    return minImageCount;
  }

  static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
    assert(!availableFormats.empty());
    const auto formatIt =
        std::ranges::find_if(availableFormats, [](const auto& format) {
          return format.format == vk::Format::eB8G8R8A8Srgb &&
                 format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
        });
    return formatIt != availableFormats.end() ? *formatIt : availableFormats[0];
  }

  static vk::PresentModeKHR chooseSwapPresentMode(
      std::vector<vk::PresentModeKHR> const& availablePresentModes) {
    assert(std::ranges::any_of(availablePresentModes, [](auto presentMode) {
      return presentMode == vk::PresentModeKHR::eFifo;
    }));
    return std::ranges::any_of(availablePresentModes,
                               [](const vk::PresentModeKHR value) {
                                 return vk::PresentModeKHR::eMailbox == value;
                               })
               ? vk::PresentModeKHR::eMailbox
               : vk::PresentModeKHR::eFifo;
  }

  vk::Extent2D chooseSwapExtent(vk::SurfaceCapabilitiesKHR const& capabilities,
                                SDL_Window* sdlwindow) {
    if (capabilities.currentExtent.width !=
        std::numeric_limits<uint32_t>::max()) {
      return capabilities.currentExtent;
    }
    int width, height;
    SDL_GetWindowSizeInPixels(sdlwindow, &width, &height);

    return {std::clamp<uint32_t>(width, capabilities.minImageExtent.width,
                                 capabilities.maxImageExtent.width),
            std::clamp<uint32_t>(height, capabilities.minImageExtent.height,
                                 capabilities.maxImageExtent.height)};
  }
};

} // namespace WisE
