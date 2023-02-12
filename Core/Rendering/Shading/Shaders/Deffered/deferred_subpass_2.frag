#version 450

layout(location = 0) in vec3 fromVert_UVW;

layout(set = 0, binding = 1) uniform samplerCube samplerCubeMap;
layout(input_attachment_index = 0, set = 0, binding = 2) uniform subpassInput fromSubpass_DepthBuffer;

layout(location = 0) out vec4 outColor;

void main()
{
    float depth = subpassLoad(fromSubpass_DepthBuffer).r;
    if (depth != 1.0)
    {
        discard;
    }

    outColor = texture(samplerCubeMap, fromVert_UVW);
}