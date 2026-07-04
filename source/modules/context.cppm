module;

#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

export module context;

namespace WisE {

export struct vulkan_cntx {
  vk::raii::Device device;
  vk::raii::PhysicalDevice& physicalDevice;
  vk::raii::DescriptorSetLayout& descriptorSetLayout;
  vk::SurfaceFormatKHR& swapChainSurfaceFormat;
  vk::raii::Pipeline& graphicsPipeline;
  vk::raii::PipelineLayout& pipelineLayout;
};

export struct InfiniteGrid {
  vk::raii::PipelineLayout pipelineLayout = nullptr;
  vk::raii::Pipeline graphicsPipeline = nullptr;

  vk::raii::Buffer vertexBuffer = nullptr;
  vk::raii::DeviceMemory vertexBufferMemory = nullptr;
  vk::raii::Buffer indexBuffer = nullptr;
  vk::raii::DeviceMemory indexBufferMemory = nullptr;
  uint32_t indexCount{0};
};

} // namespace WisE
