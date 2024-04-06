#version 450

#include "ImGuiRenderTask.glsl"

in struct
{
    float32vec4 color;
    float32vec2 UV;
} In;

SIERRA_SAMPLED_IMAGE(fontAtlas, pushConstant.fontAtlasIndex);
SIERRA_SAMPLER(fontSampler, pushConstant.fontSamplerIndex);

out float32vec4 Out;

void main()
{
    Out = In.color * texture(sampler2D(SIERRA_GET_SAMPLED_IMAGE(fontAtlas), SIERRA_GET_SAMPLER(fontSampler)), In.UV).r;
}