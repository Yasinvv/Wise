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

export vk::raii::ImageView createImageView(vk::Image const& image,
                                           vk::Format format,
                                           vk::ImageAspectFlags aspectFlags,
                                           vk::raii::Device& device) {
  vk::ImageViewCreateInfo viewInfo{
      .image = image,
      .viewType = vk::ImageViewType::e2D,
      .format = format,
      .subresourceRange = {.aspectMask = aspectFlags,
                           .baseMipLevel = 0,
                           .levelCount = 1,
                           .baseArrayLayer = 0,
                           .layerCount = 1}};
  return vk::raii::ImageView(device, viewInfo);
}

export uint32_t findMemoryType(uint32_t typeFilter,
                               vk::MemoryPropertyFlags properties,
                               vk::raii::PhysicalDevice& physicalDevice) {
  vk::PhysicalDeviceMemoryProperties memProperties =
      physicalDevice.getMemoryProperties();

  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags &
                                    properties) == properties) {
      return i;
    }
  }

  throw std::runtime_error("failed to find suitable memory type!");
}

export std::pair<vk::raii::Image, vk::raii::DeviceMemory>
createImage(uint32_t width, uint32_t height, vk::Format format,
            vk::ImageTiling tiling, vk::ImageUsageFlags usage,
            vk::MemoryPropertyFlags properties, vk::raii::Device& device,
            vk::raii::PhysicalDevice& physicalDevice) {
  vk::ImageCreateInfo imageInfo{.imageType = vk::ImageType::e2D,
                                .format = format,
                                .extent = {width, height, 1},
                                .mipLevels = 1,
                                .arrayLayers = 1,
                                .samples = vk::SampleCountFlagBits::e1,
                                .tiling = tiling,
                                .usage = usage,
                                .sharingMode = vk::SharingMode::eExclusive};

  vk::raii::Image image = vk::raii::Image(device, imageInfo);

  vk::MemoryRequirements memRequirements = image.getMemoryRequirements();
  vk::MemoryAllocateInfo allocInfo{
      .allocationSize = memRequirements.size,
      .memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits,
                                        properties, physicalDevice)};
  vk::raii::DeviceMemory imageMemory =
      vk::raii::DeviceMemory(device, allocInfo);
  image.bindMemory(imageMemory, 0);

  return {std::move(image), std::move(imageMemory)};
}

export std::pair<vk::raii::Buffer, vk::raii::DeviceMemory>
createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage,
             vk::MemoryPropertyFlags properties, vk::raii::Device& device,
             vk::raii::PhysicalDevice& physicalDevice) {
  vk::BufferCreateInfo bufferInfo{
      .size = size, .usage = usage, .sharingMode = vk::SharingMode::eExclusive};
  vk::raii::Buffer buffer = vk::raii::Buffer(device, bufferInfo);
  vk::MemoryRequirements memRequirements = buffer.getMemoryRequirements();
  vk::MemoryAllocateInfo allocInfo{
      .allocationSize = memRequirements.size,
      .memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits,
                                        properties, physicalDevice)};
  vk::raii::DeviceMemory bufferMemory =
      vk::raii::DeviceMemory(device, allocInfo);
  buffer.bindMemory(*bufferMemory, 0);
  return {std::move(buffer), std::move(bufferMemory)};
}

export void transitionImageLayout(vk::raii::CommandBuffer& commandBuffer,
                                  const vk::raii::Image& image,
                                  vk::ImageLayout oldLayout,
                                  vk::ImageLayout newLayout) {
  vk::ImageMemoryBarrier barrier{
      .oldLayout = oldLayout,
      .newLayout = newLayout,
      .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
      .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
      .image = image,
      .subresourceRange = {.aspectMask = vk::ImageAspectFlagBits::eColor,
                           .levelCount = 1,
                           .layerCount = 1}};

  vk::PipelineStageFlags sourceStage;
  vk::PipelineStageFlags destinationStage;

  if (oldLayout == vk::ImageLayout::eUndefined &&
      newLayout == vk::ImageLayout::eTransferDstOptimal) {
    barrier.srcAccessMask = {};
    barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

    sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
    destinationStage = vk::PipelineStageFlagBits::eTransfer;
  } else if (oldLayout == vk::ImageLayout::eTransferDstOptimal &&
             newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
    barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
    barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

    sourceStage = vk::PipelineStageFlagBits::eTransfer;
    destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
  } else {
    throw std::invalid_argument("unsupported layout transition!");
  }
  commandBuffer.pipelineBarrier(sourceStage, destinationStage, {}, {}, {},
                                barrier);
}

export void copyBufferToImage(vk::raii::CommandBuffer& commandBuffer,
                              const vk::raii::Buffer& buffer,
                              vk::raii::Image& image, uint32_t width,
                              uint32_t height) {
  vk::BufferImageCopy region{
      .bufferOffset = 0,
      .bufferRowLength = 0,
      .bufferImageHeight = 0,
      .imageSubresource = {.aspectMask = vk::ImageAspectFlagBits::eColor,
                           .mipLevel = 0,
                           .baseArrayLayer = 0,
                           .layerCount = 1},
      .imageOffset = {0, 0, 0},
      .imageExtent = {width, height, 1}};
  commandBuffer.copyBufferToImage(buffer, image,
                                  vk::ImageLayout::eTransferDstOptimal, region);
}

} // namespace WisE
