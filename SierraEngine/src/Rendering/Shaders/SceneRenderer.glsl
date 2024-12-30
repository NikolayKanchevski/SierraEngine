#ifndef SCENE_RENDERER_GLSL
#define SCENE_RENDERER_GLSL

#include "../../../../Sierra/include/Sierra/Sierra.glsl"

struct PushConstant
{
    mat4x4 view;
    mat4x4 projection;
};
SIERRA_PUSH_CONSTANT(pushConstant, PushConstant);

#endif