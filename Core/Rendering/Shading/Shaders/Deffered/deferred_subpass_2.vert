#version 450

#include "../shader_utilities.glsl"

layout(location = 0) in vec3 fromCode_Position;

layout(push_constant) uniform PushConstant
{
    mat4 model;
} pushConstant;

layout(location = 0) out vec3 toFrag_UVW;

void main()
{
    vec4 position = uniformBuffer.projection * mat4x4(mat3x3(uniformBuffer.view)) * pushConstant.model * vec4(fromCode_Position, 1.0);
    gl_Position = position.xyww;

    toFrag_UVW = fromCode_Position;
    toFrag_UVW.y *= -1;
}