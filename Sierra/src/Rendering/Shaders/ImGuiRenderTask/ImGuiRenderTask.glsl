#ifndef IMGUI_RENDER_TASK_GLSL
#define IMGUI_RENDER_TASK_GLSL

#include "../../../../include/Sierra.glsl"

layout(push_constant) uniform PushConstant
{
    uint32 fontAtlasIndex;
    uint32 fontSamplerIndex;

    vec2 translation;
    vec2 scale;
} pushConstant;

#endif