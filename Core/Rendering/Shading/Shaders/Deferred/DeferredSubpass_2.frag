#version 450

layout(location = 0) in vec3 fromVert_UVW;

layout(set = 0, binding = 1) uniform samplerCube samplerCubeMap;

layout(location = 2) out vec4 toFramebuffer_FinalizedColor;

void main()
{
    toFramebuffer_FinalizedColor = texture(samplerCubeMap, fromVert_UVW);
}