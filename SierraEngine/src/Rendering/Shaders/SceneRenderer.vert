#version 450

#include "SceneRenderer.glsl"

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 UV;

out struct
{
    vec3 color;
} Out;

void main()
{
    gl_Position = SIERRA_GET_PUSH_CONSTANT(pushConstant).projection * SIERRA_GET_PUSH_CONSTANT(pushConstant).view * mat4x4(1.0f) * vec4(position, 1.0);
    Out.color = normal;
}
