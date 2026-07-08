
module;

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

export module logicalDevice;
import context;

namespace WisE {

export class LogicalDevice {
private:
public:
  void createLogicalDevice(VK_CTX& ctx) {
    std::vector<vk::QueueFamilyProperties> queueFamilyProperties =
        ctx.physicalDevice.getQueueFamilyProperties();

    // get the first index into queueFamilyProperties which supports both
    // graphics and present
    for (uint32_t qfpIndex = 0; qfpIndex < queueFamilyProperties.size();
         qfpIndex++) {
      if ((queueFamilyProperties[qfpIndex].queueFlags &
           vk::QueueFlagBits::eGraphics) &&
          ctx.physicalDevice.getSurfaceSupportKHR(qfpIndex, *ctx.surface)) {
        // found a queue family that supports both graphics and present
        ctx.queueIndex = qfpIndex;
        break;
      }
    }
    if (ctx.queueIndex == vk::QueueFamilyIgnored) {
      throw std::runtime_error(
          "Could not find a queue for graphics and present -> terminating");
    }

    // query for Vulkan 1.3 features
    vk::StructureChain<vk::PhysicalDeviceFeatures2,
                       vk::PhysicalDeviceVulkan13Features,
                       vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>
        featureChain = {
            {.features = {.samplerAnisotropy =
                              true}}, // vk::PhysicalDeviceFeatures2
            {.synchronization2 = true,
             .dynamicRendering = true}, // vk::PhysicalDeviceVulkan13Features
            {.extendedDynamicState =
                 true} // vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT
        };

    // create a Device
    float queuePriority = 0.5f;
    vk::DeviceQueueCreateInfo deviceQueueCreateInfo{
        .queueFamilyIndex = ctx.queueIndex,
        .queueCount = 1,
        .pQueuePriorities = &queuePriority};
    vk::DeviceCreateInfo deviceCreateInfo{
        .pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &deviceQueueCreateInfo,
        .enabledExtensionCount =
            static_cast<uint32_t>(requiredDeviceExtension.size()),
        .ppEnabledExtensionNames = requiredDeviceExtension.data()};

    ctx.device = vk::raii::Device(ctx.physicalDevice, deviceCreateInfo);
    ctx.queue = vk::raii::Queue(ctx.device, ctx.queueIndex, 0);
  }
};
} // namespace WisE
