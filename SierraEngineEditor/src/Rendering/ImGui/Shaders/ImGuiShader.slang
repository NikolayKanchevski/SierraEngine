#include "Sierra/Sierra.slang"

struct PushConstant
{
    uint32 textureIndex;
    uint32 samplerIndex;
    float2 scale;
};
[[vk::push_constant]] ConstantBuffer<PushConstant> pushConstant;

struct VertexInput
{
    [[vk::location(0)]] float2 position;
    [[vk::location(1)]] float2 UV;
    [[vk::location(2)]] float4 color;
};

struct VertexOutput
{
    float4 position : SV_Position;
    [[vk::location(0)]] float4 color;
    [[vk::location(1)]] float2 UV;
};

[shader("vertex")]
VertexOutput vertexMain(const VertexInput input)
{
    VertexOutput output;
    output.position = float4(float2(-1.0, 1.0) + pushConstant.scale * input.position, 0.0, 1.0);
    output.color = input.color;
    output.UV = input.UV;
    return output;
}

[shader("fragment")]
float4 fragmentMain(const VertexOutput input) : SV_Target
{
    return input.color * Sierra::GetSampledImage2D(pushConstant.textureIndex).Sample(Sierra::GetSampler(pushConstant.samplerIndex), input.UV);
}
