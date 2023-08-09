#ifndef GLOBAL_UNIFORM_BUFFER_GLSL
#define GLOBAL_UNIFORM_BUFFER_GLSL

#include "../Utility/ShaderDefinitions.glsl"

layout(binding = UNIFORM_BUFFER_BINDING) uniform UniformBuffer
{
    mat4x4 view;
    mat4x4 projection;
    mat4x4 inverseView;
    mat4x4 inverseProjection;
    float nearClip;
    float farClip;
} uniformBuffer;

#endif