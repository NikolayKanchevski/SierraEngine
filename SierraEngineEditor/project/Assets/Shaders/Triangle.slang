#include "Sierra/Sierra.slang"

// Mockup of the scene renderer, which draws a single triangle with a color at each of its corners

struct PushConstant
{
    float4x4 view;
    float4x4 projection;
};
[[vk::push_constant]] ConstantBuffer<PushConstant> pushConstant;

struct VertexInput
{
    [[vk::location(0)]] float3 position;
    [[vk::location(1)]] float4 color;
};

struct VertexOutput
{
    float4 position : SV_Position;
    [[vk::location(0)]] float4 color;
};

[shader("vertex")]
VertexOutput vertexMain(const VertexInput input)
{
    VertexOutput output;
    output.position = mul(pushConstant.projection, mul(pushConstant.view, float4(input.position, 1.0)));
    output.color = input.color;
    return output;
}

[shader("fragment")]
float4 fragmentMain(const VertexOutput input) : SV_Target
{
    return input.color;
}
