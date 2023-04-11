#version 450

#include "../Utility/GaussianBlur.glsl"
#include "../Utility/ShaderDefinitions.glsl"

layout(set = 0, binding = 2) uniform sampler2D fromCode_Image;

struct PushConstant
{
    uint blurDirection;
};

SET_PUSH_CONSTANT(PushConstant);

layout(location = 0) in vec2 fromVert_UV;

layout(location = 0) out vec4 toFramebuffer_FinalizedColor;

void main()
{
    toFramebuffer_FinalizedColor = GaussianBlurTexel5x5(fromCode_Image, fromVert_UV, PUSH_CONSTANT.blurDirection);
}