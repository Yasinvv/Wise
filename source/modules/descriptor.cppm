module;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

export module descriptor;
import context;

namespace WisE {
export class Descriptor {
private:
public:
  void createDescriptorSetLayout(VK_CTX& ctx, Object_CTX& object) {
    std::array<vk::DescriptorSetLayoutBinding, 2> bindings{
        {{.binding = 0,
          .descriptorType = vk::DescriptorType::eUniformBuffer,
          .descriptorCount = 1,
          .stageFlags = vk::ShaderStageFlagBits::eVertex},
         {.binding = 1,
          .descriptorType = vk::DescriptorType::eCombinedImageSampler,
          .descriptorCount = 1,
          .stageFlags = vk::ShaderStageFlagBits::eFragment}}};

    vk::DescriptorSetLayoutCreateInfo layoutInfo{
        .bindingCount = static_cast<uint32_t>(bindings.size()),
        .pBindings = bindings.data()};
    object.materialRef->descriptorSetLayout =
        vk::raii::DescriptorSetLayout(ctx.device, layoutInfo);
  }
  void createDescriptorPool(VK_CTX& ctx, Object_CTX& object) {
    std::array<vk::DescriptorPoolSize, 2> poolSize{
        {{.type = vk::DescriptorType::eUniformBuffer,
          .descriptorCount = MAX_FRAMES_IN_FLIGHT},
         {.type = vk::DescriptorType::eCombinedImageSampler,
          .descriptorCount = MAX_FRAMES_IN_FLIGHT}}};
    vk::DescriptorPoolCreateInfo poolInfo{
        .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
        .maxSets = MAX_FRAMES_IN_FLIGHT,
        .poolSizeCount = static_cast<uint32_t>(poolSize.size()),
        .pPoolSizes = poolSize.data()};
    object.descriptorPool = vk::raii::DescriptorPool(ctx.device, poolInfo);
  }

  void createDescriptorSets(VK_CTX& ctx, Object_CTX& object) {
    std::vector<vk::DescriptorSetLayout> layouts(
        MAX_FRAMES_IN_FLIGHT, object.materialRef->descriptorSetLayout);
    vk::DescriptorSetAllocateInfo allocInfo{
        .descriptorPool = object.descriptorPool,
        .descriptorSetCount = static_cast<uint32_t>(layouts.size()),
        .pSetLayouts = layouts.data()};

    object.descriptorSets.clear();
    object.descriptorSets = ctx.device.allocateDescriptorSets(allocInfo);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      std::vector<vk::WriteDescriptorSet> descriptorWrites;

      vk::DescriptorBufferInfo bufferInfo{.buffer = object.uniformBuffers[i],
                                          .offset = 0,
                                          .range = sizeof(UniformBufferObject)};

      descriptorWrites.push_back(
          {.dstSet = object.descriptorSets[i],
           .dstBinding = 0,
           .dstArrayElement = 0,
           .descriptorCount = 1,
           .descriptorType = vk::DescriptorType::eUniformBuffer,
           .pBufferInfo = &bufferInfo});

      vk::DescriptorImageInfo imageInfo{};
      if (object.materialRef->textureImageView != nullptr) {
        imageInfo.sampler = object.materialRef->textureSampler;
        imageInfo.imageView = object.materialRef->textureImageView;
        imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

        descriptorWrites.push_back(
            {.dstSet = object.descriptorSets[i],
             .dstBinding = 1,
             .dstArrayElement = 0,
             .descriptorCount = 1,
             .descriptorType = vk::DescriptorType::eCombinedImageSampler,
             .pImageInfo = &imageInfo});
      }

      ctx.device.updateDescriptorSets(descriptorWrites, {});
    }
  }
};

} // namespace WisE
