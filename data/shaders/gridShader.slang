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

[shader("vertex")]
VertexToFragment vertexMain(VertexInput input) {
  VertexToFragment output;

  float4x4 mvp = mul(ubo.proj, mul(ubo.view, ubo.model));
  output.outPosition = mul(mvp, float4(input.pos, 1.0));

  output.outColor = input.color;
  output.outTexCoord = input.texCoord;

  return output;
}
float2 vulkan_fract(float2 x) { return x - floor(x); }

[shader("fragment")]
float4 fragMain(VertexToFragment input) : SV_Target {
  float2 coord = input.outTexCoord;

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
