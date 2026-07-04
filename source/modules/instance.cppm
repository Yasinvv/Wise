module;

#include <SDL3/SDL_vulkan.h>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

export module instance;
import context;

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif

namespace WisE {

export class Instance {

private:
  const std::vector<char const*> validationLayers = {
      "VK_LAYER_KHRONOS_validation"};

public:
  void createInstance(VK_CTX& ctx) {
    constexpr vk::ApplicationInfo appInfo{
        .pApplicationName = "preview",
        .applicationVersion = VK_MAKE_VERSION(0, 0, 1),
        .pEngineName = "Wise",
        .engineVersion = VK_MAKE_VERSION(0, 0, 1),
        .apiVersion = vk::ApiVersion14};

    // Get the required layers
    std::vector<char const*> requiredLayers;
    if (enableValidationLayers) {
      requiredLayers.assign(validationLayers.begin(), validationLayers.end());
    }

    // Check if the required layers are supported by the Vulkan
    // implementation.
    auto layerProperties = ctx.context.enumerateInstanceLayerProperties();
    auto unsupportedLayerIt = std::ranges::find_if(
        requiredLayers, [&layerProperties](auto const& requiredLayer) {
          return std::ranges::none_of(
              layerProperties, [requiredLayer](auto const& layerProperty) {
                return strcmp(layerProperty.layerName, requiredLayer) == 0;
              });
        });
    if (unsupportedLayerIt != requiredLayers.end()) {
      throw std::runtime_error("Required layer not supported: " +
                               std::string(*unsupportedLayerIt));
    }

    // Get the required extensions.
    auto requiredExtensions = getRequiredInstanceExtensions();

    // Check if the required extensions are supported by the Vulkan
    // implementation.
    auto extensionProperties =
        ctx.context.enumerateInstanceExtensionProperties();
    auto unsupportedPropertyIt = std::ranges::find_if(
        requiredExtensions,
        [&extensionProperties](auto const& requiredExtension) {
          return std::ranges::none_of(
              extensionProperties,
              [requiredExtension](auto const& extensionProperty) {
                return strcmp(extensionProperty.extensionName,
                              requiredExtension) == 0;
              });
        });
    if (unsupportedPropertyIt != requiredExtensions.end()) {
      throw std::runtime_error("Required extension not supported: " +
                               std::string(*unsupportedPropertyIt));
    }

    vk::InstanceCreateInfo createInfo{
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
        .ppEnabledLayerNames = requiredLayers.data(),
        .enabledExtensionCount =
            static_cast<uint32_t>(requiredExtensions.size()),
        .ppEnabledExtensionNames = requiredExtensions.data()};
    ctx.instance = vk::raii::Instance(ctx.context, createInfo);
  }

  [[nodiscard]] std::vector<const char*> getRequiredInstanceExtensions() {
    uint32_t SDlExtensionCount = 0;
    const char* const* SDlExtensionsNames =
        SDL_Vulkan_GetInstanceExtensions(&SDlExtensionCount);

    std::vector<const char*> extensions(SDlExtensionsNames,
                                        SDlExtensionsNames + SDlExtensionCount);

    if (enableValidationLayers) {
      extensions.push_back(vk::EXTDebugUtilsExtensionName);
    }
    return extensions;
  }
};
} // namespace WisE
