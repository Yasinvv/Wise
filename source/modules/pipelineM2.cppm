module;

#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

export module pipeline;
import extra;
import vertex;
import context;

namespace WisE {

export class Pipeline {
public:
  struct PipelineConfigs {
    std::string shaderPath{};
    std::string vertexEntryPoint = "vertMain";
    std::string fragmentEntryPoint = "fragMain";

    vk::VertexInputBindingDescription bindingDescription =
        WisE::Vertex::getBindingDescription();
    std::array<vk::VertexInputAttributeDescription, 3> attributeDescriptions =
        WisE::Vertex::getAttributeDescriptions();

    vk::PrimitiveTopology topology = vk::PrimitiveTopology::eTriangleList;
    vk::PolygonMode polygonMode = vk::PolygonMode::eFill;
    vk::CullModeFlags cullMode = vk::CullModeFlagBits::eNone;
    vk::FrontFace frontFace = vk::FrontFace::eCounterClockwise;

    vk::Bool32 depthTestEnable = vk::True;
    vk::Bool32 depthWriteEnable = vk::True;
    vk::CompareOp depthCompareOp = vk::CompareOp::eLess;

    vk::Bool32 blendEnable = vk::False;
  };

  void createGraphicsPipeline(VK_CTX& ctx, Object_CTX& object,
                              const PipelineConfigs& configs) {
    vk::raii::ShaderModule shaderModule = WisE::createShaderModule(
        WisE::readFile(configs.shaderPath), ctx.device);

    vk::PipelineShaderStageCreateInfo vertShaderStageInfo{
        .stage = vk::ShaderStageFlagBits::eVertex,
        .module = *shaderModule,
        .pName = configs.vertexEntryPoint.c_str()};

    vk::PipelineShaderStageCreateInfo fragShaderStageInfo{
        .stage = vk::ShaderStageFlagBits::eFragment,
        .module = *shaderModule,
        .pName = configs.fragmentEntryPoint.c_str()};

    vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo,
                                                        fragShaderStageInfo};

    vk::PipelineVertexInputStateCreateInfo vertexInputInfo{
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &configs.bindingDescription,
        .vertexAttributeDescriptionCount =
            static_cast<uint32_t>(configs.attributeDescriptions.size()),
        .pVertexAttributeDescriptions = configs.attributeDescriptions.data()};

    vk::PipelineInputAssemblyStateCreateInfo inputAssembly{
        .topology = configs.topology};
    vk::PipelineViewportStateCreateInfo viewportState{.viewportCount = 1,
                                                      .scissorCount = 1};

    vk::PipelineRasterizationStateCreateInfo rasterizer{
        .depthClampEnable = vk::False,
        .rasterizerDiscardEnable = vk::False,
        .polygonMode = configs.polygonMode,
        .cullMode = configs.cullMode,
        .frontFace = configs.frontFace,
        .depthBiasEnable = vk::False,
        .lineWidth = 1.0f};

    vk::PipelineMultisampleStateCreateInfo multisampling{
        .rasterizationSamples = vk::SampleCountFlagBits::e1,
        .sampleShadingEnable = vk::False};

    vk::PipelineDepthStencilStateCreateInfo depthStencil{
        .depthTestEnable = configs.depthTestEnable,
        .depthWriteEnable = configs.depthWriteEnable,
        .depthCompareOp = configs.depthCompareOp,
        .depthBoundsTestEnable = vk::False,
        .stencilTestEnable = vk::False};

    vk::PipelineColorBlendAttachmentState colorBlendAttachment{
        .blendEnable = configs.blendEnable,
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
        .pSetLayouts = &*(object.materialRef->descriptorSetLayout),
        .pushConstantRangeCount = 0};
    object.materialRef->pipelineLayout =
        vk::raii::PipelineLayout(ctx.device, pipelineLayoutInfo);

    vk::Format depthFormat = WisE::findDepthFormat(ctx.physicalDevice);

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
             .layout = object.materialRef->pipelineLayout,
             .renderPass = nullptr},
            {.colorAttachmentCount = 1,
             .pColorAttachmentFormats = &ctx.swapChainSurfaceFormat.format,
             .depthAttachmentFormat = depthFormat}};

    object.materialRef->graphicsPipeline = vk::raii::Pipeline(
        ctx.device, nullptr,
        pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>());
  }
};
} // namespace WisE
