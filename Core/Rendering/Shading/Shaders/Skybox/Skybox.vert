#version 450

#include "../Utility/Cube.glsl"

layout(binding = 0) uniform UniformBuffer
{
    mat4 view;
    mat4 projection;
} uniformBuffer;

layout(push_constant) uniform PushConstant
{
    mat4 model;
} pushConstant;

layout(location = 0) out vec3 toFrag_UVW;

void main()
{
    vec4 position = uniformBuffer.projection * mat4x4(mat3x3(uniformBuffer.view)) * pushConstant.model * vec4(cubeVertices[gl_VertexIndex], 1.0);
    gl_Position = position.xyww;

    toFrag_UVW = cubeVertices[gl_VertexIndex];
    toFrag_UVW.y *= -1;
}