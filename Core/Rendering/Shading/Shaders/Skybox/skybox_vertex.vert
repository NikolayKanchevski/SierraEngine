#version 450

layout(location = 0) in vec3 fromCode_Position;

layout(set = 0, binding = 0) uniform UniformBuffer
{
    mat4 view;
    mat4 projection;
    mat4 model;
} uniformBuffer;

layout(location = 0) out vec3 toFrag_UVW;

void main()
{
    gl_Position = uniformBuffer.projection * uniformBuffer.view * uniformBuffer.model * vec4(fromCode_Position, 1.0);
    toFrag_UVW = fromCode_Position;
    toFrag_UVW.y *= -1;
}