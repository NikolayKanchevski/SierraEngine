#ifndef DEPTH_UTILITIES_GLSL
#define DEPTH_UTILITIES_GLSL

vec3 WorldPositionFromDepth(vec2 UV, float depth, mat4 invProjectMatrix, mat4 invViewMatrix)
{
    vec4 clipSpacePosition = vec4(UV * 2.0 - 1.0, depth, 1.0);
    vec4 viewSpacePosition = invProjectMatrix * clipSpacePosition;

    viewSpacePosition /= viewSpacePosition.w;
    vec4 worldSpacePosition = invViewMatrix * viewSpacePosition;

    return worldSpacePosition.xyz;
}

vec3 ViewPositionFromDepth(vec2 UV, float depth, mat4 invProjectMatrix)
{
    vec4 clipSpacePosition = vec4(UV * 2.0 - 1.0, depth, 1.0);
    vec4 viewSpacePosition = invProjectMatrix * clipSpacePosition;

    viewSpacePosition /= viewSpacePosition.w;

    return viewSpacePosition.xyz;
}

vec3 WorldPositionFromDepth(vec2 fragmentPosition, vec2 screenArea, float depth, mat4 inverseProjectMatrix, mat4 inverseViewMatrix)
{
    return WorldPositionFromDepth(fragmentPosition / screenArea, depth, inverseProjectMatrix, inverseViewMatrix);
}

#endif