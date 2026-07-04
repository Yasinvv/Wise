module;

#include <algorithm>
#include <cstring>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

export module physicalDevice;
import context;

namespace WisE {
export class PhysicalDevice {
public:
  bool isDeviceSuitable(const vk::raii::PhysicalDevice& pDevice,
                        const VK_CTX& ctx) {
    bool supportsVulkan1_4 =
        pDevice.getProperties().apiVersion >= VK_API_VERSION_1_4;

    auto queueFamilies = pDevice.getQueueFamilyProperties();
    bool supportsGraphics =
        std::ranges::any_of(queueFamilies, [](auto const& qfp) {
          return !!(qfp.queueFlags & vk::QueueFlagBits::eGraphics);
        });

    auto availableDeviceExtensions =
        pDevice.enumerateDeviceExtensionProperties();
    bool supportsAllRequiredExtensions = std::ranges::all_of(
        ctx.requiredDeviceExtension,
        [&availableDeviceExtensions](auto const& requiredExt) {
          return std::ranges::any_of(availableDeviceExtensions,
                                     [requiredExt](auto const& availableExt) {
                                       return strcmp(availableExt.extensionName,
                                                     requiredExt) == 0;
                                     });
        });

    auto features = pDevice.template getFeatures2<
        vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan13Features,
        vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();
    bool supportsRequiredFeatures =
        features.template get<vk::PhysicalDeviceFeatures2>()
            .features.samplerAnisotropy &&
        features.template get<vk::PhysicalDeviceVulkan13Features>()
            .dynamicRendering &&
        features
            .template get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>()
            .extendedDynamicState;

    return supportsVulkan1_4 && supportsGraphics &&
           supportsAllRequiredExtensions && supportsRequiredFeatures;
  }

  void pickPhysicalDevice(VK_CTX& ctx) {
    std::vector<vk::raii::PhysicalDevice> physicalDevices =
        ctx.instance.enumeratePhysicalDevices();
    auto const devIter =
        std::ranges::find_if(physicalDevices, [&](auto const& pDevice) {
          return isDeviceSuitable(pDevice, ctx);
        });
    if (devIter == physicalDevices.end()) {
      throw std::runtime_error("failed to find a suitable GPU!");
    }
    ctx.physicalDevice = std::move(*devIter);
  }
};
} // namespace WisE
