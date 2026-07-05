module;

#include <string>
#include <vulkan/vulkan.hpp>

#include <vulkan/vulkan_raii.hpp>

export module context;

namespace WisE {

export struct Configs {
  uint8_t MaxFPS{30U};
};

export struct VK_CTX {
  std::vector<const char*> requiredDeviceExtension = {
      vk::KHRSwapchainExtensionName};

  vk::raii::DescriptorPool imGuiDescriptorPool{nullptr};
  vk::raii::Context context;
  vk::raii::Instance instance{nullptr};
  vk::raii::DebugUtilsMessengerEXT debugMessenger{nullptr};
  vk::raii::SurfaceKHR surface{nullptr};
  vk::raii::PhysicalDevice physicalDevice{nullptr};
  vk::raii::Device device{nullptr};
  vk::raii::Queue queue{nullptr};
  uint32_t queueIndex = ~0;
  vk::raii::SwapchainKHR swapChain{nullptr};
  std::vector<vk::Image> swapChainImages;
  vk::SurfaceFormatKHR swapChainSurfaceFormat;
  vk::Extent2D swapChainExtent;
  std::vector<vk::raii::ImageView> swapChainImageViews;

  vk::raii::DescriptorSetLayout descriptorSetLayout{nullptr};

  vk::raii::Image depthImage{nullptr};
  vk::raii::DeviceMemory depthImageMemory{nullptr};
  vk::raii::ImageView depthImageView{nullptr};

  vk::raii::Image textureImage{nullptr};
  vk::raii::DeviceMemory textureImageMemory{nullptr};
  vk::raii::ImageView textureImageView{nullptr};
  vk::raii::Sampler textureSampler{nullptr};

  vk::raii::Buffer vertexBuffer{nullptr};
  vk::raii::DeviceMemory vertexBufferMemory{nullptr};
  vk::raii::Buffer indexBuffer{nullptr};
  vk::raii::DeviceMemory indexBufferMemory{nullptr};

  std::vector<vk::raii::Buffer> uniformBuffers;
  std::vector<vk::raii::DeviceMemory> uniformBuffersMemory;
  std::vector<void*> uniformBuffersMapped;

  vk::raii::DescriptorPool descriptorPool{nullptr};
  std::vector<vk::raii::DescriptorSet> descriptorSets;
  // vk::raii::PipelineLayout pipelineLayout{nullptr};
  // vk::raii::Pipeline graphicsPipeline{nullptr};

  vk::raii::CommandPool commandPool{nullptr};
  std::vector<vk::raii::CommandBuffer> commandBuffers;

  std::vector<vk::raii::Semaphore> presentCompleteSemaphores;
  std::vector<vk::raii::Semaphore> renderFinishedSemaphores;
  std::vector<vk::raii::Fence> inFlightFences;
  uint32_t frameIndex = 0;
};

export struct Path {
  std::string MODEL_PATH{};
  std::string TEXTURE_PATH{};
};

export struct InfiniteGrid {
  vk::raii::PipelineLayout pipelineLayout{nullptr};
  vk::raii::Pipeline graphicsPipeline{nullptr};

  vk::raii::Buffer vertexBuffer{nullptr};
  vk::raii::DeviceMemory vertexBufferMemory{nullptr};
  vk::raii::Buffer indexBuffer{nullptr};
  vk::raii::DeviceMemory indexBufferMemory{nullptr};
  uint32_t indexCount{0};
};
} // namespace WisE
