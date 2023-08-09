#version 450

#include "../Utility/GaussianBlur.glsl"
#include "../Utility/ShaderDefinitions.glsl"

layout(binding = 2) uniform sampler2D fromCode_Image;

layout(push_constant) uniform PushConstant
{
    uint blurDirection;
} pushConstant;

layout(location = 0) in vec2 fromVert_UV;

layout(location = 0) out vec4 toFramebuffer_FinalizedColor;

void main()
{
    toFramebuffer_FinalizedColor = GaussianBlurTexel5x5(fromCode_Image, fromVert_UV, pushConstant.blurDirection);
}