#version 450

layout(location = 0) in vec3 fromVert_UVW;

layout(set = 0, binding = 2) uniform samplerCube samplerCubeMap;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = texture(samplerCubeMap, fromVert_UVW);
}