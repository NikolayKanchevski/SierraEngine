#include "Sierra/Sierra.slang"

struct PushConstant
{
    float4x4 view;
    float4x4 projection;
};
[[vk::push_constant]] ConstantBuffer<PushConstant> pushConstant;

struct VertexInput
{
    [[vk::location(0)]] float3 position;
    [[vk::location(1)]] float3 normal;
    [[vk::location(2)]] float2 UV;
};

struct VertexOutput
{
    float4 position : SV_Position;
    [[vk::location(0)]] float3 color;
    [[vk::location(1)]] float2 UV;
};

[shader("vertex")]
VertexOutput vertexMain(const VertexInput input)
{
    VertexOutput output;
    output.position = mul(pushConstant.projection, mul(pushConstant.view, float4(input.position, 1.0)));
    output.color = input.normal;
    output.UV = input.UV;
    return output;
}

[shader("fragment")]
float4 fragmentMain(const VertexOutput input) : SV_Target
{
    return float4(input.color, 1.0);
}
