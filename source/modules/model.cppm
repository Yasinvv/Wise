module;

#include <tiny_obj_loader.h>
#include <unordered_map>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

export module model;
import context;
import extra;
import vertex;

namespace WisE {
export class Model {
private:
public:
  void loadModel(Path& path, Model_CTX& model) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
                          path.MODEL_PATH.c_str())) {
      throw std::runtime_error(warn + err);
    }

    std::unordered_map<Vertex, uint32_t> uniqueVertices{};

    for (const auto& shape : shapes) {
      for (const auto& index : shape.mesh.indices) {
        WisE::Vertex vertex{};

        vertex.pos = {attrib.vertices[3 * index.vertex_index + 0],
                      attrib.vertices[3 * index.vertex_index + 1],
                      attrib.vertices[3 * index.vertex_index + 2]};

        vertex.texCoord = {attrib.texcoords[2 * index.texcoord_index + 0],
                           1.0f -
                               attrib.texcoords[2 * index.texcoord_index + 1]};

        vertex.color = {1.0f, 1.0f, 1.0f};

#if 1
        auto [it, inserted] = uniqueVertices.insert(
            {vertex, static_cast<uint32_t>(model.vertices.size())});
        if (inserted) {
          model.vertices.push_back(vertex);
        }

        model.indices.push_back(it->second);
#else
        vertices.push_back(vertex);
        indices.push_back(static_cast<uint32_t>(indices.size()));
#endif
      }
    }
  }
};
} // namespace WisE
