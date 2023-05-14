#ifndef DEPTH_UTILITIES_GLSL
#define DEPTH_UTILITIES_GLSL

vec3 GetWorldPositionFromDepth(vec2 UV, float depth, mat4x4 inverseProjectionMatrix, mat4x4 inverseViewMatrix)
{
    vec4 clipSpacePosition = vec4(UV * 2.0 - 1.0, depth, 1.0);
    vec4 viewSpacePosition = inverseProjectionMatrix * clipSpacePosition;

    viewSpacePosition /= viewSpacePosition.w;
    vec4 worldSpacePosition = inverseViewMatrix * viewSpacePosition;

    return worldSpacePosition.xyz;
}

vec3 GetViewPositionFromDepth(vec2 UV, float depth, mat4x4 invProjectionMatrix)
{
    vec4 clipSpacePosition = vec4(UV * 2.0 - 1.0, depth, 1.0);
    vec4 viewSpacePosition = invProjectionMatrix * clipSpacePosition;

    viewSpacePosition /= viewSpacePosition.w;

    return viewSpacePosition.xyz;
}

vec3 GetWorldPositionFromDepth(vec2 fragmentPosition, vec2 screenArea, float depth, mat4x4 inverseProjectionMatrix, mat4x4 inverseViewMatrix)
{
    return GetWorldPositionFromDepth(fragmentPosition / screenArea, depth, inverseProjectionMatrix, inverseViewMatrix);
}

float GetDepthFromWorldPosition(vec3 position, mat4x4 projectionMatrix)
{
    vec3 absPos = abs(position);
    float z = -max(absPos.x, max(absPos.y, absPos.z));
    vec4 clip = projectionMatrix * vec4(0.0, 0.0, z, 1.0);
    return (clip.z / clip.w) * 0.5 + 0.5;
}

float LinearizeDepth(float depth, float zNear, float zFar)
{
    return (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
}

#endif