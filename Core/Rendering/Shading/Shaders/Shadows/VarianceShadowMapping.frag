#version 450

layout(location = 0) out vec2 toFramebuffer_VarianceMap;

void main()
{
    const float originalDepth = gl_FragCoord.z;
    toFramebuffer_VarianceMap.x = originalDepth;
    toFramebuffer_VarianceMap.y = originalDepth * originalDepth;
}