struct UniformBufferObject {
  float4x4 model;
  float4x4 view;
  float4x4 proj;
};

[[vk::binding(0, 0)]]
ConstantBuffer<UniformBufferObject> ubo;

struct VertexInput {
  float3 pos : POSITION;
  float3 color : COLOR;
  float2 texCoord : TEXCOORD;
};

struct VertexToFragment {
  float4 outPosition : SV_Position;
  float3 outColor : COLOR;
  float2 outTexCoord : TEXCOORD;
};
float hash(float2 p) {
  p = frac(p * float2(127.1, 311.7));
  return frac(sin(dot(p, float2(12.9898, 78.233))) * 43758.5453123);
}

float noise(float2 p) {
  float2 i = floor(p);
  float2 f = frac(p);
  float2 u = f * f * (3.0 - 2.0 * f);

  return lerp(lerp(hash(i + float2(0.0, 0.0)), hash(i + float2(1.0, 0.0)), u.x),
              lerp(hash(i + float2(0.0, 1.0)), hash(i + float2(1.0, 1.0)), u.x),
              u.y);
}
// -------------------------------------------------------------

[shader("vertex")]
VertexToFragment vertexMain(VertexInput input) {
  VertexToFragment output;

  float3 modifiedPos = input.pos;

  float terrainScale = 0.05;
  float terrainHeight = 8.0;

  modifiedPos.z += noise(modifiedPos.xy * terrainScale) * terrainHeight;

  float4x4 mvp = mul(ubo.proj, mul(ubo.view, ubo.model));
  output.outPosition = mul(mvp, float4(modifiedPos, 1.0));

  output.outColor = input.color;
  output.outTexCoord = input.texCoord;

  return output;
}

float2 vulkan_fract(float2 x) { return x - floor(x); }

[shader("fragment")]
float4 fragMain(VertexToFragment input) : SV_Target {

  //
  //
  //
  //
  //
  //
  float squareSize = 0.1;
  float2 coord = input.outTexCoord / squareSize;

  float2 derivative = fwidth(coord);
  float2 grid = abs(vulkan_fract(coord - 0.5) - 0.5) / derivative;
  float line = min(grid.x, grid.y);

  float minimum_width = 2.0;
  float alpha = 1.0 - min(line / minimum_width, 1.0);

  float2 check = vulkan_fract(floor(coord) * 0.5);
  bool isEven = (check.x == check.y);

  float4 baseColor =
      isEven ? float4(0.3, 0.3, 0.3, 1.0) : float4(0.2, 0.2, 0.2, 1.0);
  float4 lineColor = float4(1.0, 1.0, 1.0, 0.3);
  float4 finalColor = lerp(baseColor, lineColor, alpha);

  float target_distance = length(coord);
  float fade = 1.0 - smoothstep(10.0, 80.0, target_distance);
  finalColor.a *= fade;

  return finalColor;
}

void createGridMesh() {
  std::vector<Vertex> gridVertices;
  std::vector<uint32_t> gridIndices;

  const int GRID_SIZE = 100;
  const float MESH_SIZE = 100.0f;
  const float halfSize = MESH_SIZE / 2.0f;
  const float step = MESH_SIZE / GRID_SIZE;

  for (int y = 0; y <= GRID_SIZE; ++y) {
    for (int x = 0; x <= GRID_SIZE; ++x) {
      Vertex v;
      float posX = -halfSize + (x * step);
      float posY = -halfSize + (y * step);
      v.pos = { posX, posY, 0.0f };

      v.color = { 1.0f, 1.0f, 1.0f };

      v.texCoord = { posX, posY };

      gridVertices.push_back(v);
    }
  }

  for (int y = 0; y < GRID_SIZE; ++y) {
    for (int x = 0; x < GRID_SIZE; ++x) {
      uint32_t row1 = y * (GRID_SIZE + 1);
      uint32_t row2 = (y + 1) * (GRID_SIZE + 1);

      gridIndices.push_back(row1 + x);
      gridIndices.push_back(row1 + x + 1);
      gridIndices.push_back(row2 + x);

      gridIndices.push_back(row2 + x);
      gridIndices.push_back(row1 + x + 1);
      gridIndices.push_back(row2 + x + 1);
    }
  }

  m_infiniteGrid.indexCount = static_cast<uint32_t>(gridIndices.size());

  vk::DeviceSize vertexBufferSize =
      sizeof(gridVertices[0]) * gridVertices.size();
  vk::DeviceSize indexBufferSize = sizeof(gridIndices[0]) * gridIndices.size();

  {
    auto [stagingBuffer, stagingBufferMemory] =
        createBuffer(vertexBufferSize, vk::BufferUsageFlagBits::eTransferSrc,
                     vk::MemoryPropertyFlagBits::eHostVisible |
                         vk::MemoryPropertyFlagBits::eHostCoherent);
    void* data = stagingBufferMemory.mapMemory(0, vertexBufferSize);
    std::memcpy(data, gridVertices.data(),
                static_cast<size_t>(vertexBufferSize));
    stagingBufferMemory.unmapMemory();

    std::tie(m_infiniteGrid.vertexBuffer, m_infiniteGrid.vertexBufferMemory) =
        createBuffer(vertexBufferSize,
                     vk::BufferUsageFlagBits::eTransferDst |
                         vk::BufferUsageFlagBits::eVertexBuffer,
                     vk::MemoryPropertyFlagBits::eDeviceLocal);
    copyBuffer(stagingBuffer, m_infiniteGrid.vertexBuffer, vertexBufferSize);
  }

  {
    auto [stagingBuffer, stagingBufferMemory] =
        createBuffer(indexBufferSize, vk::BufferUsageFlagBits::eTransferSrc,
                     vk::MemoryPropertyFlagBits::eHostVisible |
                         vk::MemoryPropertyFlagBits::eHostCoherent);
    void* data = stagingBufferMemory.mapMemory(0, indexBufferSize);
    std::memcpy(data, gridIndices.data(), static_cast<size_t>(indexBufferSize));
    stagingBufferMemory.unmapMemory();

    std::tie(m_infiniteGrid.indexBuffer, m_infiniteGrid.indexBufferMemory) =
        createBuffer(indexBufferSize,
                     vk::BufferUsageFlagBits::eTransferDst |
                         vk::BufferUsageFlagBits::eIndexBuffer,
                     vk::MemoryPropertyFlagBits::eDeviceLocal);
    copyBuffer(stagingBuffer, m_infiniteGrid.indexBuffer, indexBufferSize);
  }
}
