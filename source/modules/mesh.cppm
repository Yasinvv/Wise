module;

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

export module mesh;
import context;
import extra;
import vertex;
import commandBuffer;

namespace WisE {

export class Mesh {

private:
public:
  void createMesh(VK_CTX& ctx, Object_CTX& object,
                  CommandBuffer& m_commandBuffer) {
    std::vector<Vertex> gridVertices = {
        {{-500.0f, -500.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {-500.0f, -500.0f}},
        {{500.0f, -500.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {500.0f, -500.0f}},
        {{500.0f, 500.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {500.0f, 500.0f}},
        {{-500.0f, 500.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {-500.0f, 500.0f}}};

    std::vector<uint32_t> gridIndices = {0, 1, 2, 2, 3, 0};

    object.indexCount = static_cast<uint32_t>(gridIndices.size());

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

      std::tie(object.vertexBuffer, object.vertexBufferMemory) =
          createBuffer(vertexBufferSize,
                       vk::BufferUsageFlagBits::eTransferDst |
                           vk::BufferUsageFlagBits::eVertexBuffer,
                       vk::MemoryPropertyFlagBits::eDeviceLocal, ctx.device,
                       ctx.physicalDevice);

      m_commandBuffer.copyBuffer(stagingBuffer, object.vertexBuffer,
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

      std::tie(object.indexBuffer, object.indexBufferMemory) =
          createBuffer(indexBufferSize,
                       vk::BufferUsageFlagBits::eTransferDst |
                           vk::BufferUsageFlagBits::eIndexBuffer,
                       vk::MemoryPropertyFlagBits::eDeviceLocal, ctx.device,
                       ctx.physicalDevice);

      m_commandBuffer.copyBuffer(stagingBuffer, object.indexBuffer,
                                 indexBufferSize, ctx);
    }
  }
};
} // namespace WisE
