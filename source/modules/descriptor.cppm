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
  void createDescriptorSetLayout(VK_CTX& ctx) {
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
    ctx.descriptorSetLayout =
        vk::raii::DescriptorSetLayout(ctx.device, layoutInfo);
  }
  void createDescriptorPool(VK_CTX& ctx) {
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
    ctx.descriptorPool = vk::raii::DescriptorPool(ctx.device, poolInfo);
  }

  void createDescriptorSets(VK_CTX& ctx) {
    std::vector<vk::DescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT,
                                                 ctx.descriptorSetLayout);
    vk::DescriptorSetAllocateInfo allocInfo{
        .descriptorPool = ctx.descriptorPool,
        .descriptorSetCount = static_cast<uint32_t>(layouts.size()),
        .pSetLayouts = layouts.data()};

    ctx.descriptorSets.clear();
    ctx.descriptorSets = ctx.device.allocateDescriptorSets(allocInfo);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      vk::DescriptorBufferInfo bufferInfo{.buffer = ctx.uniformBuffers[i],
                                          .offset = 0,
                                          .range = sizeof(UniformBufferObject)};
      vk::DescriptorImageInfo imageInfo{
          .sampler = ctx.textureSampler,
          .imageView = ctx.textureImageView,
          .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal};

      std::array<vk::WriteDescriptorSet, 2> descriptorWrites{
          {{.dstSet = ctx.descriptorSets[i],
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = vk::DescriptorType::eUniformBuffer,
            .pBufferInfo = &bufferInfo},
           {.dstSet = ctx.descriptorSets[i],
            .dstBinding = 1,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = vk::DescriptorType::eCombinedImageSampler,
            .pImageInfo = &imageInfo}}};
      ctx.device.updateDescriptorSets(descriptorWrites, {});
    }
  }
};

} // namespace WisE
