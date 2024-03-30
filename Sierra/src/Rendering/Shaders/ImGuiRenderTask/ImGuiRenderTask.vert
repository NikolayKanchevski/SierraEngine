#version 450

#include "ImGuiRenderTask.glsl"

in float32vec2 position;
in float32vec2 UV;
in float32vec4 color;

out struct
{
    float32vec4 color;
    float32vec2 UV;
} Out;

void main()
{
    gl_Position = vec4(pushConstant.translation + pushConstant.scale * position, 0, 1);

    Out.color = color;
    Out.UV = UV;
}