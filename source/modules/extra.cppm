module;

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

export module extra;

namespace WisE {

export std::vector<char> readFile(const std::string& filename) {
  std::ifstream file(filename, std::ios::ate | std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error("failed to open file!");
  }
  std::vector<char> buffer(file.tellg());
  file.seekg(0, std::ios::beg);
  file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
  file.close();
  return buffer;
}

export vk::Format
findSupportedFormat(const std::vector<vk::Format>& candidates,
                    vk::ImageTiling tiling, vk::FormatFeatureFlags features,
                    vk::raii::PhysicalDevice& physicalDevice) {
  for (const auto format : candidates) {
    vk::FormatProperties props = physicalDevice.getFormatProperties(format);
    if (((tiling == vk::ImageTiling::eLinear) &&
         ((props.linearTilingFeatures & features) == features)) ||
        ((tiling == vk::ImageTiling::eOptimal) &&
         ((props.optimalTilingFeatures & features) == features))) {
      return format;
    }
  }

  throw std::runtime_error("failed to find supported format!");
}

export [[nodiscard]] vk::Format
findDepthFormat(vk::raii::PhysicalDevice& physicalDevice) {
  return findSupportedFormat(
      {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint,
       vk::Format::eD24UnormS8Uint},
      vk::ImageTiling::eOptimal,
      vk::FormatFeatureFlagBits::eDepthStencilAttachment, physicalDevice);
}
export [[nodiscard]] vk::raii::ShaderModule
createShaderModule(const std::vector<char>& code, vk::raii::Device& device) {
  vk::ShaderModuleCreateInfo createInfo{
      .codeSize = code.size(),
      .pCode = reinterpret_cast<const uint32_t*>(code.data())};
  vk::raii::ShaderModule shaderModule{device, createInfo};

  return shaderModule;
}
} // namespace WisE
