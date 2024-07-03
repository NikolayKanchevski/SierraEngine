#ifndef IMGUI_RENDER_TASK_GLSL
#define IMGUI_RENDER_TASK_GLSL

#include "../../../../include/Sierra/Sierra.glsl"

layout(push_constant) uniform PushConstant
{
    uint32 textureIndex;
    uint32 samplerIndex;
    vec2 scale;
} pushConstant;

#endif