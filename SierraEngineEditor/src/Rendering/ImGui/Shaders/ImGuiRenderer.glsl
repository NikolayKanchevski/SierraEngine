#ifndef IMGUI_RENDERER_GLSL
#define IMGUI_RENDERER_GLSL

#include "../../../../include/Sierra/Sierra.glsl"

struct PushConstant
{
    uint32 textureIndex;
    uint32 samplerIndex;
    vec2 scale;
};

SIERRA_PUSH_CONSTANT(pushConstant, PushConstant);

#endif