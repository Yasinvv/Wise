module;

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_vulkan.h"
#include <cassert>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

export module imGUI;
import context;
import extra;
import window;

namespace WisE {

export class ImGUI {
private:
public:
  void initImGui(VK_CTX& ctx, Window& window) {
    std::array<vk::DescriptorPoolSize, 6> poolSizes{
        {{vk::DescriptorType::eSampler, 1000},
         {vk::DescriptorType::eCombinedImageSampler, 1000},
         {vk::DescriptorType::eSampledImage, 1000},
         {vk::DescriptorType::eStorageImage, 1000},
         {vk::DescriptorType::eUniformBuffer, 1000},
         {vk::DescriptorType::eStorageBuffer, 1000}}};

    vk::DescriptorPoolCreateInfo poolInfo{
        .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
        .maxSets = 1000,
        .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
        .pPoolSizes = poolSizes.data()};

    ctx.imGuiDescriptorPool = vk::raii::DescriptorPool(ctx.device, poolInfo);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplSDL3_InitForVulkan(window.window);

    ImGui_ImplVulkan_InitInfo initInfo = {};
    initInfo.Instance = *ctx.instance;
    initInfo.PhysicalDevice = *ctx.physicalDevice;
    initInfo.Device = *ctx.device;

    initInfo.QueueFamily = 0;
    initInfo.Queue = *vk::raii::Queue(ctx.device, initInfo.QueueFamily, 0);

    initInfo.DescriptorPool = *ctx.imGuiDescriptorPool;
    initInfo.MinImageCount = 2;
    initInfo.ImageCount = static_cast<uint32_t>(ctx.swapChainImages.size());

    initInfo.UseDynamicRendering = true;

    VkFormat colorFmt =
        static_cast<VkFormat>(ctx.swapChainSurfaceFormat.format);

    VkPipelineRenderingCreateInfo dynamicRenderingInfo{};
    dynamicRenderingInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    dynamicRenderingInfo.colorAttachmentCount = 1;
    dynamicRenderingInfo.pColorAttachmentFormats = &colorFmt;
    dynamicRenderingInfo.depthAttachmentFormat =
        static_cast<VkFormat>(findDepthFormat(ctx.physicalDevice));

    initInfo.PipelineInfoMain.PipelineRenderingCreateInfo =
        dynamicRenderingInfo;
    initInfo.ApiVersion = VK_API_VERSION_1_3;

    ImGui_ImplVulkan_Init(&initInfo);
  }
};
} // namespace WisE
