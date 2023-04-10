#ifndef DEPTH_UTILITIES_GLSL
#define DEPTH_UTILITIES_GLSL

vec3 GetWorldPositionFromDepth(vec2 UV, float depth, mat4x4 inverseProjectMatrix, mat4x4 inverseViewMatrix)
{
    vec4 clipSpacePosition = vec4(UV * 2.0 - 1.0, depth, 1.0);
    vec4 viewSpacePosition = inverseProjectMatrix * clipSpacePosition;

    viewSpacePosition /= viewSpacePosition.w;
    vec4 worldSpacePosition = inverseViewMatrix * viewSpacePosition;

    return worldSpacePosition.xyz;
}

vec3 GetViewPositionFromDepth(vec2 UV, float depth, mat4x4 invProjectMatrix)
{
    vec4 clipSpacePosition = vec4(UV * 2.0 - 1.0, depth, 1.0);
    vec4 viewSpacePosition = invProjectMatrix * clipSpacePosition;

    viewSpacePosition /= viewSpacePosition.w;

    return viewSpacePosition.xyz;
}

vec3 GetWorldPositionFromDepth(vec2 fragmentPosition, vec2 screenArea, float depth, mat4x4 inverseProjectMatrix, mat4x4 inverseViewMatrix)
{
    return GetWorldPositionFromDepth(fragmentPosition / screenArea, depth, inverseProjectMatrix, inverseViewMatrix);
}

#endif