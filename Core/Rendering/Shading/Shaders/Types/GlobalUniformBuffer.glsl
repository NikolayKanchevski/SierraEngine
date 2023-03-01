#ifndef GLOBAL_UNIFORM_BUFFER
#define GLOBAL_UNIFORM_BUFFER

#include "../Utility/ShaderDefinitions.glsl"

layout(set = 0, binding = UNIFORM_BUFFER_BINDING) uniform UniformBuffer
{
    mat4 view;
    mat4 projection;
    mat4 inverseView;
    mat4 inverseProjection;
} uniformBuffer;

#endif