#ifndef SHADOWS_GLSL
#define SHADOWS_GLSL

#include "../Utility/Math.glsl"

// NOTE: Shadow maps' dimensions must be in a 1:1 aspect

const float SHADOW_BIAS = 0.001f;
const float SHADOW_TRANSPARENCY = 0.75f;

float CalculateShadow(sampler2D shadowMap, vec4 shadowCoordinates, vec2 offset)
{
    shadowCoordinates = shadowCoordinates / shadowCoordinates.w;
    shadowCoordinates.xy = shadowCoordinates.xy * 0.5 + 0.5;

    const float dist = texture(shadowMap, shadowCoordinates.xy + offset).r;
    return shadowCoordinates.w > 0.0 && dist < shadowCoordinates.z ? 0.0 : 1.0;
}

const uint PCF_SAMPLE_COUNT = 1;

float CalculateShadowPCF(sampler2DShadow shadowMap, vec4 shadowCoordinates, int filterSize)
{
    const float resolution = textureSize(shadowMap, 0).x;
    const float texelSize = 1.0 / resolution;

    shadowCoordinates = shadowCoordinates / shadowCoordinates.w;
    shadowCoordinates.xy = shadowCoordinates.xy * 0.5 + 0.5;

    float shadow = 0.0f;
    vec2 grad = fract(shadowCoordinates.xy * resolution + 0.5f);

    for (int x = -filterSize; x <= filterSize; x++)
    {
        for (int y = -filterSize; y <= filterSize; y++)
        {
            float pcfDepth = texture(shadowMap, shadowCoordinates.xyz + vec3(vec2(x * texelSize, y * texelSize), -SHADOW_BIAS)).r;
            shadow += pcfDepth;
        }
    }

    return shadow / float((2 * filterSize + 1) * float(2 * filterSize + 1));
}

float CalculateShadow3x3PCF(sampler2DShadow shadowMap, vec4 shadowCoordinates)
{
    return CalculateShadowPCF(shadowMap, shadowCoordinates, 1);
}

float CalculateShadow5x5PCF(sampler2DShadow shadowMap, vec4 shadowCoordinates)
{
    return CalculateShadowPCF(shadowMap, shadowCoordinates, 2);
}

float CalculateShadow7x7PCF(sampler2DShadow shadowMap, vec4 shadowCoordinates)
{
    return CalculateShadowPCF(shadowMap, shadowCoordinates, 3);
}

float CalculateShadow9x9PCF(sampler2DShadow shadowMap, vec4 shadowCoordinates)
{
    return CalculateShadowPCF(shadowMap, shadowCoordinates, 4);
}

const uint POISSON_SAMPLE_COUNT = 4;
const float POISSON_RADIUS = 2.0f;
const float POISSON_SHARPNESS = 0.1f;

float CalculatePoissonShadowPCF(sampler2DShadow shadowMap, vec4 shadowCoordinates, vec3 fragmentPosition)
{
    const float texelSize = 1.0 / textureSize(shadowMap, 0).x;

    shadowCoordinates = shadowCoordinates / shadowCoordinates.w;
    shadowCoordinates.xy = shadowCoordinates.xy * 0.5 + 0.5;

    float shadow = 0.0;
    for (int i = 0; i < POISSON_SAMPLE_COUNT; i++)
    {
        uint index;

        #if defined(SETTINGS_USE_RANDOM_SAMPLE_FOR_POISSON)
            // No banding, but shadow moves with camera, causing a weird visual
            index = int(16.0 * Random(fragmentPosition, i)) % 16;
        #else
            // Gives a fixed pattern, but without noise
            index = i;
        #endif

        vec2 offset;
        #if defined(SETTINGS_USE_GRADIENT_SAMPLING_FOR_POISSON)
            const vec3 magic = vec3(0.06711056, 0.00583715, 52.9829189);
            const float rotationX = cos(2.0 * PI * fract(magic.z * fract(dot(gl_FragCoord.xy, magic.xy))));
            const float rotationY = sin(2.0 * PI * fract(magic.z * fract(dot(gl_FragCoord.xy, magic.xy))));
            const mat2x2 rotationMatrix = mat2x2(rotationX, rotationY, -rotationY, rotationX);
            offset = rotationMatrix * poissonDisk[index];
        #else
            offset = poissonDisk[index] * Random(vec3(poissonDisk[index], 1.0), i) * PI;
        #endif

        shadow += texture(shadowMap, shadowCoordinates.xyz + vec3(texelSize * POISSON_RADIUS * offset, -SHADOW_BIAS)).r;
    }

    return shadow / POISSON_SAMPLE_COUNT;
}

#endif