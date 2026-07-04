module;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

export module vertex;

namespace WisE {

export struct Vertex {
  glm::vec3 pos;
  glm::vec3 color;
  glm::vec2 texCoord;

  static vk::VertexInputBindingDescription getBindingDescription() {
    return {.binding = 0,
            .stride = sizeof(Vertex),
            .inputRate = vk::VertexInputRate::eVertex};
  }

  static std::array<vk::VertexInputAttributeDescription, 3>
  getAttributeDescriptions() {
    return {{{.location = 0,
              .binding = 0,
              .format = vk::Format::eR32G32B32Sfloat,
              .offset = offsetof(Vertex, pos)},
             {.location = 1,
              .binding = 0,
              .format = vk::Format::eR32G32B32Sfloat,
              .offset = offsetof(Vertex, color)},
             {.location = 2,
              .binding = 0,
              .format = vk::Format::eR32G32Sfloat,
              .offset = offsetof(Vertex, texCoord)}}};
  }

  bool operator==(const Vertex& other) const {
    return pos == other.pos && color == other.color &&
           texCoord == other.texCoord;
  }
};
} // namespace WisE
export template <> struct std::hash<WisE::Vertex> {
  size_t operator()(WisE::Vertex const& vertex) const noexcept {
    return ((hash<glm::vec3>()(vertex.pos) ^
             (hash<glm::vec3>()(vertex.color) << 1)) >>
            1) ^
           (hash<glm::vec2>()(vertex.texCoord) << 1);
  }
};
