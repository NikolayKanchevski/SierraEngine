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

const float32vec2 TRANSLATION = vec2(-1.0, 1.0);
void main()
{
    gl_Position = vec4(TRANSLATION + pushConstant.scale * position, 0.0, 1.0);

    Out.color = color;
    Out.UV = UV;
}