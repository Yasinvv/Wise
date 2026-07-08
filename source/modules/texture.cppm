module;

#include "stb_image.h"
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

export module texture;
import context;
import extra;
import commandBuffer;

namespace WisE {
export class Texture {
private:
public:
  void createTextureImage(VK_CTX& ctx, Object_CTX& object,
                          CommandBuffer& m_commandBuffer, Path& path) {
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(path.TEXTURE_PATH.c_str(), &texWidth,
                                &texHeight, &texChannels, STBI_rgb_alpha);
    vk::DeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels) {
      throw std::runtime_error("failed to load texture image!");
    }

    auto [stagingBuffer, stagingBufferMemory] =
        createBuffer(imageSize, vk::BufferUsageFlagBits::eTransferSrc,
                     vk::MemoryPropertyFlagBits::eHostVisible |
                         vk::MemoryPropertyFlagBits::eHostCoherent,
                     ctx.device, ctx.physicalDevice);

    void* data = stagingBufferMemory.mapMemory(0, imageSize);
    memcpy(data, pixels, imageSize);
    stagingBufferMemory.unmapMemory();

    stbi_image_free(pixels);

    std::tie(object.materialRef->textureImage,
             object.materialRef->textureImageMemory) =
        createImage(texWidth, texHeight, vk::Format::eR8G8B8A8Srgb,
                    vk::ImageTiling::eOptimal,
                    vk::ImageUsageFlagBits::eTransferDst |
                        vk::ImageUsageFlagBits::eSampled,
                    vk::MemoryPropertyFlagBits::eDeviceLocal, ctx.device,
                    ctx.physicalDevice);

    vk::raii::CommandBuffer commandBuffer =
        m_commandBuffer.beginSingleTimeCommands(ctx);
    transitionImageLayout(commandBuffer, object.materialRef->textureImage,
                          vk::ImageLayout::eUndefined,
                          vk::ImageLayout::eTransferDstOptimal);
    copyBufferToImage(
        commandBuffer, stagingBuffer, object.materialRef->textureImage,
        static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
    transitionImageLayout(commandBuffer, object.materialRef->textureImage,
                          vk::ImageLayout::eTransferDstOptimal,
                          vk::ImageLayout::eShaderReadOnlyOptimal);
    m_commandBuffer.endSingleTimeCommands(std::move(commandBuffer), ctx);
  }
  void createTextureImageView(VK_CTX& ctx, Object_CTX& object) {
    object.materialRef->textureImageView = createImageView(
        *object.materialRef->textureImage, vk::Format::eR8G8B8A8Srgb,
        vk::ImageAspectFlagBits::eColor, ctx.device);
  }

  void createTextureSampler(VK_CTX& ctx, Object_CTX& object) {
    vk::PhysicalDeviceProperties properties =
        ctx.physicalDevice.getProperties();
    vk::SamplerCreateInfo samplerInfo{
        .magFilter = vk::Filter::eLinear,
        .minFilter = vk::Filter::eLinear,
        .mipmapMode = vk::SamplerMipmapMode::eLinear,
        .addressModeU = vk::SamplerAddressMode::eRepeat,
        .addressModeV = vk::SamplerAddressMode::eRepeat,
        .addressModeW = vk::SamplerAddressMode::eRepeat,
        .mipLodBias = 0.0f,
        .anisotropyEnable = vk::True,
        .maxAnisotropy = properties.limits.maxSamplerAnisotropy,
        .compareEnable = vk::False,
        .compareOp = vk::CompareOp::eAlways};
    object.materialRef->textureSampler =
        vk::raii::Sampler(ctx.device, samplerInfo);
  }
};

} // namespace WisE
