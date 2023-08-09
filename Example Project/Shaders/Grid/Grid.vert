#version 450

#include "../Types/GlobalUniformBuffer.glsl"

vec3 gridPlane[6] = vec3[](
    vec3( 1, -1,  0),
    vec3(-1,  1,  0),
    vec3(-1, -1,  0),
    vec3(-1,  1,  0),
    vec3( 1, -1,  0),
    vec3( 1,  1,  0)
);

layout(location = 1) out vec3 toFrag_NearPoint;
layout(location = 2) out vec3 toFrag_FarPoint;

vec3 UnprojectPoint(float x, float y, float z)
{
    vec4 unprojectedPoint = uniformBuffer.inverseView * uniformBuffer.inverseProjection * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main()
{
    vec3 point = gridPlane[gl_VertexIndex].xyz;

    toFrag_NearPoint = UnprojectPoint(point.x, point.y, 0.0).xyz; // unprojecting on the near plane
    toFrag_FarPoint = UnprojectPoint(point.x, point.y, 1.0).xyz; // unprojecting on the far plane

    gl_Position = vec4(point, 1.0); // using directly the clipped coordinates
}