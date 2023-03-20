#version 450

#include "../Types/GlobalUniformBuffer.glsl"

layout(location = 1) in vec3 fromVert_NearPoint;
layout(location = 2) in vec3 fromVert_FarPoint;

layout(location = 0) out vec4 toFramebuffer_Color;

const float depthNear = 0.01f;
const float depthFar = 75.0f;

vec4 Grid(vec3 fragmentPosition, float spacing)
{
    // Calculate coordinates and line width
    const vec2 coord = fragmentPosition.xz * spacing;
    const vec2 derivative = fwidth(coord) * 2.0f;
    const vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    const float line = min(grid.x, grid.y);
    const float zMinimum = min(derivative.y, 1.0f);
    const float xMinimum = min(derivative.x, 1.0f);

    vec4 color = vec4(0.2f, 0.2f, 0.2f, 1.0f - min(line, 1.0f));

    // Z Axis color
    if (fragmentPosition.x > -0.1f * xMinimum && fragmentPosition.x < 0.1f * xMinimum)
    {
        color.rgb = vec3(0.8f, 0.1f, 0.15f);
    }

    // X Axis color
    if (fragmentPosition.z > -0.1f * zMinimum && fragmentPosition.z < 0.1f * zMinimum)
    {
        color.rgb = vec3(0.1f, 0.25f, 0.8f);
    }

    return color;
}


float CalculateDepth(vec3 position)
{
    vec4 clipSpacePosition = uniformBuffer.projection * uniformBuffer.view * vec4(position.xyz, 1.0);
    return (clipSpacePosition.z / clipSpacePosition.w);
}

float CalculateLinearDepth(vec3 position)
{
    const vec4 clipSpacePosition = uniformBuffer.projection * uniformBuffer.view * vec4(position.xyz, 1.0);
    const float clipSpaceDepth = (clipSpacePosition.z / clipSpacePosition.w) * 2.0 - 1.0;
    const float linearDepth = (2.0 * depthNear * depthFar) / (depthFar + depthNear - clipSpaceDepth * (depthFar - depthNear));
    return linearDepth / depthFar;
}

void main()
{
    /*
        Equation of a line: y = nearPoint.y + t * (farPoint.y - nearPoint.y),
        so for y = 0 we have: t = -nearPoint.y / (farPoint.y - nearPoint.y)
    */

    // Calculate fragment position
    const float t = -fromVert_NearPoint.y / (fromVert_FarPoint.y - fromVert_NearPoint.y);
    const vec3 fragmentPosition = fromVert_NearPoint + t * (fromVert_FarPoint - fromVert_NearPoint);

    // Define fade strength using depth values

    gl_FragDepth = CalculateDepth(fragmentPosition);
    const vec4 clipSpacePosition = uniformBuffer.projection * uniformBuffer.view * vec4(fragmentPosition.xyz, 1.0);
    const float clipSpaceDepth = (clipSpacePosition.z / clipSpacePosition.w) * 2.0 - 1.0;
    if (clipSpaceDepth >= 1.0f)
    {
        discard;
    }
    const float fading = max(0.0f, (0.5f - CalculateLinearDepth(fragmentPosition)));

    // Calculate final color
    vec4 finalColor = (Grid(fragmentPosition, 3) + Grid(fragmentPosition, 1)) * float(t > 0); // adding multiple resolution for the grid
    finalColor.a *= fading;

    // Write color to image
    toFramebuffer_Color = finalColor;
}