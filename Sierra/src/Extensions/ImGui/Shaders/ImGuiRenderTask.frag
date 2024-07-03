#version 450

#include "ImGuiRenderTask.glsl"

in struct
{
    float32vec4 color;
    float32vec2 UV;
} In;

SIERRA_SAMPLED_IMAGE_2D(currentTexture, pushConstant.textureIndex);
SIERRA_SAMPLER(currentSampler, pushConstant.samplerIndex);

out float32vec4 Out;

void main()
{
    Out = In.color * texture(sampler2D(SIERRA_GET_SAMPLED_IMAGE_2D(currentTexture), SIERRA_GET_SAMPLER(currentSampler)), In.UV);
}