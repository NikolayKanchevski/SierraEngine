#version 450

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 UV;
layout(location = 2) in vec4 color;

layout(push_constant) uniform PushConstant
{
    vec2 translation;
    vec2 scale;
} pushConstant;

layout(location = 0) out struct
{
    vec4 color;
    vec2 UV;
} Out;

void main()
{
    gl_Position = vec4(pushConstant.translation + pushConstant.scale * position, 0, 1);

    Out.color = color;
    Out.UV = UV;
}