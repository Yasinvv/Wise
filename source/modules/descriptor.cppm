module;

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
};

} // namespace WisE
