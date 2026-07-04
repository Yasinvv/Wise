module;

#include <algorithm>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

export module physicalDevice;

namespace WisE {

bool isDeviceSuitable(vk::raii::PhysicalDevice const& physicalDevice,
                      std::vector<const char*>& requiredDeviceExtension) {
  // Check if the physicalDevice supports the Vulkan 1.4 API version
  bool supportsVulkan1_3 =
      physicalDevice.getProperties().apiVersion >= VK_API_VERSION_1_4;

  // Check if any of the queue families support graphics operations
  auto queueFamilies = physicalDevice.getQueueFamilyProperties();
  bool supportsGraphics =
      std::ranges::any_of(queueFamilies, [](auto const& qfp) {
        return !!(qfp.queueFlags & vk::QueueFlagBits::eGraphics);
      });

  // Check if all required physicalDevice extensions are available
  auto availableDeviceExtensions =
      physicalDevice.enumerateDeviceExtensionProperties();
  bool supportsAllRequiredExtensions = std::ranges::all_of(
      requiredDeviceExtension,
      [&availableDeviceExtensions](auto const& requiredDeviceExtension) {
        return std::ranges::any_of(
            availableDeviceExtensions,
            [requiredDeviceExtension](auto const& availableDeviceExtension) {
              return strcmp(availableDeviceExtension.extensionName,
                            requiredDeviceExtension) == 0;
            });
      });

  // Check if the physicalDevice supports the required features
  auto features = physicalDevice.template getFeatures2<
      vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan13Features,
      vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();
  bool supportsRequiredFeatures =
      features.template get<vk::PhysicalDeviceFeatures2>()
          .features.samplerAnisotropy &&
      features.template get<vk::PhysicalDeviceVulkan13Features>()
          .dynamicRendering &&
      features.template get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>()
          .extendedDynamicState;

  // Return true if the physicalDevice meets all the criteria
  return supportsVulkan1_3 && supportsGraphics &&
         supportsAllRequiredExtensions && supportsRequiredFeatures;
}

export void
pickPhysicalDevice(vk::raii::Instance& instance,
                   vk::raii::PhysicalDevice& physicalDevice,
                   std::vector<const char*>& requiredDeviceExtension) {
  std::vector<vk::raii::PhysicalDevice> physicalDevices =
      instance.enumeratePhysicalDevices();
  auto const devIter =
      std::ranges::find_if(physicalDevices, [&](auto const& physicalDevice) {
        return isDeviceSuitable(physicalDevice, requiredDeviceExtension);
      });
  if (devIter == physicalDevices.end()) {
    throw std::runtime_error("failed to find a suitable GPU!");
  }
  physicalDevice = *devIter;
}
} // namespace WisE
