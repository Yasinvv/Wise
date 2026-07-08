module;

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

export module grid;
import context;
import extra;
import vertex;
import commandBuffer;

namespace WisE {

export class Grid {

private:
public:
  void createGridMesh(VK_CTX& ctx, Object_CTX& m_infiniteGrid,
                      CommandBuffer& m_commandBuffer) {
    std::vector<Vertex> gridVertices = {
        {{-50.0f, -50.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {-50.0f, -50.0f}},
        {{50.0f, -50.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {50.0f, -50.0f}},
        {{50.0f, 50.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {50.0f, 50.0f}},
        {{-50.0f, 50.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {-50.0f, 50.0f}}};

    std::vector<uint32_t> gridIndices = {0, 1, 2, 2, 3, 0};

    m_infiniteGrid.indexCount = static_cast<uint32_t>(gridIndices.size());

    vk::DeviceSize vertexBufferSize =
        sizeof(gridVertices[0]) * gridVertices.size();
    vk::DeviceSize indexBufferSize =
        sizeof(gridIndices[0]) * gridIndices.size();

    {
      auto [stagingBuffer, stagingBufferMemory] =
          createBuffer(vertexBufferSize, vk::BufferUsageFlagBits::eTransferSrc,
                       vk::MemoryPropertyFlagBits::eHostVisible |
                           vk::MemoryPropertyFlagBits::eHostCoherent,
                       ctx.device, ctx.physicalDevice);

      void* data = stagingBufferMemory.mapMemory(0, vertexBufferSize);
      std::memcpy(data, gridVertices.data(),
                  static_cast<size_t>(vertexBufferSize));
      stagingBufferMemory.unmapMemory();

      std::tie(m_infiniteGrid.vertexBuffer, m_infiniteGrid.vertexBufferMemory) =
          createBuffer(vertexBufferSize,
                       vk::BufferUsageFlagBits::eTransferDst |
                           vk::BufferUsageFlagBits::eVertexBuffer,
                       vk::MemoryPropertyFlagBits::eDeviceLocal, ctx.device,
                       ctx.physicalDevice);

      m_commandBuffer.copyBuffer(stagingBuffer, m_infiniteGrid.vertexBuffer,
                                 vertexBufferSize, ctx);
    }

    {
      auto [stagingBuffer, stagingBufferMemory] =
          createBuffer(indexBufferSize, vk::BufferUsageFlagBits::eTransferSrc,
                       vk::MemoryPropertyFlagBits::eHostVisible |
                           vk::MemoryPropertyFlagBits::eHostCoherent,
                       ctx.device, ctx.physicalDevice);

      void* data = stagingBufferMemory.mapMemory(0, indexBufferSize);
      std::memcpy(data, gridIndices.data(),
                  static_cast<size_t>(indexBufferSize));
      stagingBufferMemory.unmapMemory();

      std::tie(m_infiniteGrid.indexBuffer, m_infiniteGrid.indexBufferMemory) =
          createBuffer(indexBufferSize,
                       vk::BufferUsageFlagBits::eTransferDst |
                           vk::BufferUsageFlagBits::eIndexBuffer,
                       vk::MemoryPropertyFlagBits::eDeviceLocal, ctx.device,
                       ctx.physicalDevice);

      m_commandBuffer.copyBuffer(stagingBuffer, m_infiniteGrid.indexBuffer,
                                 indexBufferSize, ctx);
    }
  }

  void createGridPipeline(VK_CTX& ctx, Object_CTX& m_infiniteGrid) {
    auto shaderCode = WisE::readFile("data/shaders/grid.spv");
    vk::raii::ShaderModule shaderModule =
        createShaderModule(shaderCode, ctx.device);

    vk::PipelineShaderStageCreateInfo vertShaderStageInfo{
        .stage = vk::ShaderStageFlagBits::eVertex,
        .module = *shaderModule,
        .pName = "vertexMain"};

    vk::PipelineShaderStageCreateInfo fragShaderStageInfo{
        .stage = vk::ShaderStageFlagBits::eFragment,
        .module = *shaderModule,
        .pName = "fragMain"};

    vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo,
                                                        fragShaderStageInfo};

    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();

    vk::PipelineVertexInputStateCreateInfo vertexInputInfo{
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &bindingDescription,
        .vertexAttributeDescriptionCount =
            static_cast<uint32_t>(attributeDescriptions.size()),
        .pVertexAttributeDescriptions = attributeDescriptions.data()};

    vk::PipelineInputAssemblyStateCreateInfo inputAssembly{
        .topology = vk::PrimitiveTopology::eTriangleList};

    vk::PipelineViewportStateCreateInfo viewportState{.viewportCount = 1,
                                                      .scissorCount = 1};

    vk::PipelineRasterizationStateCreateInfo rasterizer{
        .depthClampEnable = vk::False,
        .rasterizerDiscardEnable = vk::False,
        .polygonMode = vk::PolygonMode::eFill,
        .cullMode = vk::CullModeFlagBits::eBack,
        .frontFace = vk::FrontFace::eCounterClockwise,
        .depthBiasEnable = vk::False,
        .lineWidth = 1.0f};

    vk::PipelineMultisampleStateCreateInfo multisampling{
        .rasterizationSamples = vk::SampleCountFlagBits::e1,
        .sampleShadingEnable = vk::False};

    vk::PipelineDepthStencilStateCreateInfo depthStencil{
        .depthTestEnable = vk::True,
        .depthWriteEnable = vk::False,
        .depthCompareOp = vk::CompareOp::eLessOrEqual,
        .depthBoundsTestEnable = vk::False,
        .stencilTestEnable = vk::False};

    vk::PipelineColorBlendAttachmentState colorBlendAttachment{
        .blendEnable = vk::True,
        .srcColorBlendFactor = vk::BlendFactor::eSrcAlpha,
        .dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha,
        .colorBlendOp = vk::BlendOp::eAdd,
        .srcAlphaBlendFactor = vk::BlendFactor::eOne,
        .dstAlphaBlendFactor = vk::BlendFactor::eZero,
        .alphaBlendOp = vk::BlendOp::eAdd,
        .colorWriteMask =
            vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
            vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA};

    vk::PipelineColorBlendStateCreateInfo colorBlending{
        .logicOpEnable = vk::False,
        .logicOp = vk::LogicOp::eCopy,
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachment};

    std::vector<vk::DynamicState> dynamicStates = {vk::DynamicState::eViewport,
                                                   vk::DynamicState::eScissor};
    vk::PipelineDynamicStateCreateInfo dynamicState{
        .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
        .pDynamicStates = dynamicStates.data()};

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{
        .setLayoutCount = 1,
        .pSetLayouts = &*m_infiniteGrid.materialRef->descriptorSetLayout,
        .pushConstantRangeCount = 0};
    m_infiniteGrid.materialRef->pipelineLayout =
        vk::raii::PipelineLayout(ctx.device, pipelineLayoutInfo);

    vk::Format depthFormat = findDepthFormat(ctx.physicalDevice);

    vk::StructureChain<vk::GraphicsPipelineCreateInfo,
                       vk::PipelineRenderingCreateInfo>
        pipelineCreateInfoChain = {
            {.stageCount = 2,
             .pStages = shaderStages,
             .pVertexInputState = &vertexInputInfo,
             .pInputAssemblyState = &inputAssembly,
             .pViewportState = &viewportState,
             .pRasterizationState = &rasterizer,
             .pMultisampleState = &multisampling,
             .pDepthStencilState = &depthStencil,
             .pColorBlendState = &colorBlending,
             .pDynamicState = &dynamicState,
             .layout = *m_infiniteGrid.materialRef->pipelineLayout,
             .renderPass = nullptr},
            {.colorAttachmentCount = 1,
             .pColorAttachmentFormats = &ctx.swapChainSurfaceFormat.format,
             .depthAttachmentFormat = depthFormat}};

    m_infiniteGrid.materialRef->graphicsPipeline = vk::raii::Pipeline(
        ctx.device, nullptr,
        pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>());
  }
};
} // namespace WisE
