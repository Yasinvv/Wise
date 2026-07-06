
module;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <utility>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

export module commandBuffer;
import context;
import extra;
import camera;
import vertex;

namespace WisE {
export class CommandBuffer {
private:
public:
  void createCommandBuffers(VK_CTX& ctx) {
    ctx.commandBuffers.clear();
    vk::CommandBufferAllocateInfo allocInfo{
        .commandPool = ctx.commandPool,
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = MAX_FRAMES_IN_FLIGHT};
    ctx.commandBuffers = vk::raii::CommandBuffers(ctx.device, allocInfo);
  }

  vk::raii::CommandBuffer beginSingleTimeCommands(VK_CTX& ctx) {
    vk::CommandBufferAllocateInfo allocInfo{
        .commandPool = ctx.commandPool,
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1};
    vk::raii::CommandBuffer commandBuffer =
        std::move(vk::raii::CommandBuffers(ctx.device, allocInfo).front());

    vk::CommandBufferBeginInfo beginInfo{
        .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit};
    commandBuffer.begin(beginInfo);

    return commandBuffer;
  }
  void endSingleTimeCommands(vk::raii::CommandBuffer&& commandBuffer,
                             VK_CTX& ctx) {
    commandBuffer.end();

    vk::SubmitInfo submitInfo{.commandBufferCount = 1,
                              .pCommandBuffers = &*commandBuffer};
    ctx.queue.submit(submitInfo, nullptr);
    ctx.queue.waitIdle();
  }
  void copyBuffer(vk::raii::Buffer& srcBuffer, vk::raii::Buffer& dstBuffer,
                  vk::DeviceSize size, VK_CTX& ctx) {
    vk::raii::CommandBuffer commandCopyBuffer = beginSingleTimeCommands(ctx);
    commandCopyBuffer.copyBuffer(*srcBuffer, *dstBuffer,
                                 vk::BufferCopy{.size = size});
    endSingleTimeCommands(std::move(commandCopyBuffer), ctx);
  }
  void createVertexBuffer(VK_CTX& ctx, Model_CTX& model) {
    vk::DeviceSize bufferSize =
        sizeof(model.vertices[0]) * model.vertices.size();

    auto [stagingBuffer, stagingBufferMemory] =
        createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
                     vk::MemoryPropertyFlagBits::eHostVisible |
                         vk::MemoryPropertyFlagBits::eHostCoherent,
                     ctx.device, ctx.physicalDevice);

    void* dataStaging = stagingBufferMemory.mapMemory(0, bufferSize);
    memcpy(dataStaging, model.vertices.data(), bufferSize);
    stagingBufferMemory.unmapMemory();

    std::tie(ctx.vertexBuffer, ctx.vertexBufferMemory) =
        createBuffer(bufferSize,
                     vk::BufferUsageFlagBits::eVertexBuffer |
                         vk::BufferUsageFlagBits::eTransferDst,
                     vk::MemoryPropertyFlagBits::eDeviceLocal, ctx.device,
                     ctx.physicalDevice);

    copyBuffer(stagingBuffer, ctx.vertexBuffer, bufferSize, ctx);
  }

  void createIndexBuffer(VK_CTX& ctx, Model_CTX& model) {
    vk::DeviceSize bufferSize = sizeof(model.indices[0]) * model.indices.size();

    auto [stagingBuffer, stagingBufferMemory] =
        createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
                     vk::MemoryPropertyFlagBits::eHostVisible |
                         vk::MemoryPropertyFlagBits::eHostCoherent,
                     ctx.device, ctx.physicalDevice);

    void* data = stagingBufferMemory.mapMemory(0, bufferSize);
    memcpy(data, model.indices.data(), (size_t)bufferSize);
    stagingBufferMemory.unmapMemory();

    std::tie(ctx.indexBuffer, ctx.indexBufferMemory) =
        createBuffer(bufferSize,
                     vk::BufferUsageFlagBits::eIndexBuffer |
                         vk::BufferUsageFlagBits::eTransferDst,
                     vk::MemoryPropertyFlagBits::eDeviceLocal, ctx.device,
                     ctx.physicalDevice);

    copyBuffer(stagingBuffer, ctx.indexBuffer, bufferSize, ctx);
  }
  void createUniformBuffers(VK_CTX& ctx) {
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      vk::DeviceSize bufferSize = sizeof(UniformBufferObject);
      auto [buffer, bufferMem] =
          createBuffer(bufferSize, vk::BufferUsageFlagBits::eUniformBuffer,
                       vk::MemoryPropertyFlagBits::eHostVisible |
                           vk::MemoryPropertyFlagBits::eHostCoherent,
                       ctx.device, ctx.physicalDevice);
      ctx.uniformBuffers.emplace_back(std::move(buffer));
      ctx.uniformBuffersMemory.emplace_back(std::move(bufferMem));
      ctx.uniformBuffersMapped.emplace_back(
          ctx.uniformBuffersMemory.back().mapMemory(0, bufferSize));
    }
  }
  void updateUniformBuffer(uint32_t currentImage, VK_CTX& ctx, Camera& camera) {
    //[[maybe_unused]] float time{timer.getTime()};

    UniformBufferObject ubo{};
    ubo.model = rotate(glm::mat4(1.0f), glm::radians(-90.0f),
                       glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(camera.settings.pos,
                           camera.settings.pos + camera.settings.front,
                           camera.settings.up);
    ubo.proj =
        glm::perspective(glm::radians(45.0f),
                         static_cast<float>(ctx.swapChainExtent.width) /
                             static_cast<float>(ctx.swapChainExtent.height),
                         0.1f, 25.0f);
    ubo.proj[1][1] *= -1;

    memcpy(ctx.uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
  }

  void recordCommandBuffer(uint32_t imageIndex, VK_CTX& ctx,
                           InfiniteGrid& m_infiniteGrid, Model_CTX& model) {
    auto& commandBuffer = ctx.commandBuffers[ctx.frameIndex];
    commandBuffer.begin({});

    transition_image_layout(ctx, ctx.swapChainImages[imageIndex],
                            vk::ImageLayout::eUndefined,
                            vk::ImageLayout::eColorAttachmentOptimal, {},
                            vk::AccessFlagBits2::eColorAttachmentWrite,
                            vk::PipelineStageFlagBits2::eColorAttachmentOutput,
                            vk::PipelineStageFlagBits2::eColorAttachmentOutput,
                            vk::ImageAspectFlagBits::eColor);

    transition_image_layout(ctx, *ctx.depthImage, vk::ImageLayout::eUndefined,
                            vk::ImageLayout::eDepthAttachmentOptimal,
                            vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
                            vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
                            vk::PipelineStageFlagBits2::eEarlyFragmentTests |
                                vk::PipelineStageFlagBits2::eLateFragmentTests,
                            vk::PipelineStageFlagBits2::eEarlyFragmentTests |
                                vk::PipelineStageFlagBits2::eLateFragmentTests,
                            vk::ImageAspectFlagBits::eDepth);

    vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
    vk::ClearValue clearDepth = vk::ClearDepthStencilValue(1.0f, 0);

    vk::RenderingAttachmentInfo colorAttachmentInfo = {
        .imageView = ctx.swapChainImageViews[imageIndex],
        .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
        .loadOp = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eStore,
        .clearValue = clearColor};

    vk::RenderingAttachmentInfo depthAttachmentInfo = {
        .imageView = ctx.depthImageView,
        .imageLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal,
        .loadOp = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eStore,
        .clearValue = clearDepth};

    vk::RenderingInfo renderingInfo = {
        .renderArea = {.offset = {0, 0}, .extent = ctx.swapChainExtent},
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachmentInfo,
        .pDepthAttachment = &depthAttachmentInfo};

    commandBuffer.beginRendering(renderingInfo);

    commandBuffer.setViewport(
        0, vk::Viewport(
               0.0f, 0.0f, static_cast<float>(ctx.swapChainExtent.width),
               static_cast<float>(ctx.swapChainExtent.height), 0.0f, 1.0f));
    commandBuffer.setScissor(
        0, vk::Rect2D(vk::Offset2D(0, 0), ctx.swapChainExtent));

    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                               *ctx.graphicsPipeline);
    commandBuffer.bindVertexBuffers(0, *ctx.vertexBuffer, {0});
    commandBuffer.bindIndexBuffer(
        *ctx.indexBuffer, 0,
        vk::IndexTypeValue<decltype(model.indices)::value_type>::value);
    commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics, ctx.pipelineLayout, 0,
        *ctx.descriptorSets[ctx.frameIndex], nullptr);
    commandBuffer.drawIndexed(static_cast<uint32_t>(model.indices.size()), 1, 0,
                              0, 0);

    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                               *m_infiniteGrid.graphicsPipeline);
    commandBuffer.bindVertexBuffers(0, *m_infiniteGrid.vertexBuffer, {0});
    commandBuffer.bindIndexBuffer(*m_infiniteGrid.indexBuffer, 0,
                                  vk::IndexType::eUint32);

    commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics, *m_infiniteGrid.pipelineLayout, 0,
        *ctx.descriptorSets[ctx.frameIndex], nullptr);

    commandBuffer.drawIndexed(m_infiniteGrid.indexCount, 1, 0, 0, 0);
    commandBuffer.endRendering();

    /*transition_image_layout(swapChainImages[imageIndex],
                            vk::ImageLayout::eColorAttachmentOptimal,
                            vk::ImageLayout::ePresentSrcKHR,
                            vk::AccessFlagBits2::eColorAttachmentWrite, {},
                            vk::PipelineStageFlagBits2::eColorAttachmentOutput,
                            vk::PipelineStageFlagBits2::eBottomOfPipe,
                            vk::ImageAspectFlagBits::eColor);*/

    transition_image_layout(ctx, ctx.swapChainImages[imageIndex],
                            vk::ImageLayout::eColorAttachmentOptimal,
                            vk::ImageLayout::ePresentSrcKHR,
                            vk::AccessFlagBits2::eColorAttachmentWrite,
                            vk::AccessFlagBits2::eNone,
                            vk::PipelineStageFlagBits2::eColorAttachmentOutput,
                            vk::PipelineStageFlagBits2::eBottomOfPipe,
                            vk::ImageAspectFlagBits::eColor);
    commandBuffer.end();
  }

  void transition_image_layout(VK_CTX& ctx, vk::Image image,
                               vk::ImageLayout old_layout,
                               vk::ImageLayout new_layout,
                               vk::AccessFlags2 src_access_mask,
                               vk::AccessFlags2 dst_access_mask,
                               vk::PipelineStageFlags2 src_stage_mask,
                               vk::PipelineStageFlags2 dst_stage_mask,
                               vk::ImageAspectFlags image_aspect_flags) {
    vk::ImageMemoryBarrier2 barrier = {
        .srcStageMask = src_stage_mask,
        .srcAccessMask = src_access_mask,
        .dstStageMask = dst_stage_mask,
        .dstAccessMask = dst_access_mask,
        .oldLayout = old_layout,
        .newLayout = new_layout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image,
        .subresourceRange = {.aspectMask = image_aspect_flags,
                             .baseMipLevel = 0,
                             .levelCount = 1,
                             .baseArrayLayer = 0,
                             .layerCount = 1}};
    vk::DependencyInfo dependency_info = {.dependencyFlags = {},
                                          .imageMemoryBarrierCount = 1,
                                          .pImageMemoryBarriers = &barrier};
    ctx.commandBuffers[ctx.frameIndex].pipelineBarrier2(dependency_info);
  }
};

} // namespace WisE
