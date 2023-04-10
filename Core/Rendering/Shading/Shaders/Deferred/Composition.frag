#version 450

#include "../Utility/Depth.glsl"
#include "../Utility/Shadows.glsl"
#include "../Lighting/DefaultLighting.glsl"
#include "../Types/GlobalStorageBuffer.glsl"
#include "../Types/GlobalUniformBuffer.glsl"

layout(location = 0) in flat uint fromVert_DrawingSkybox;
layout(location = 1) in vec3 fromVert_UVW;

layout(set = 0, binding = 2) uniform sampler2D fromCode_DiffuseBuffer;
layout(set = 0, binding = 3) uniform sampler2D toFramebuffer_SpecularAndShinines;
layout(set = 0, binding = 4) uniform sampler2D fromCode_NormalBuffer;
#if defined(SETTINGS_USE_POISSON_PCF_SHADOWS) || defined(SETTINGS_USE_PCF_SHADOWS)
    layout(set = 0, binding = 5) uniform sampler2DShadow fromCode_ShadowMap;
#else
    layout(set = 0, binding = 5) uniform sampler2D fromCode_ShadowMap;
#endif
layout(set = 0, binding = 6) uniform sampler2D fromCode_DepthBuffer;
layout(set = 0, binding = 7) uniform samplerCube fromCode_Skybox;

const uint RenderedImageValue_RenderedImage = 0;
const uint RenderedImageValue_PositionBuffer = 1;
const uint RenderedImageValue_DiffuseBuffer = 2;
const uint RenderedImageValue_SpecularBuffer = 3;
const uint RenderedImageValue_ShininessBuffer = 4;
const uint RenderedImageValue_NormalBuffer = 5;
const uint RenderedImageValue_ShadowBuffer = 6;
const uint RenderedImageValue_DepthBuffer = 7;

struct PushConstant
{
    mat4x4 skyboxModel;
    mat4x4 lightSpaceMatrix;

    uint renderedImageValue;
    uint enableShadows;
};

SET_PUSH_CONSTANT(PushConstant);

layout (location = 0) out vec4 toFramebuffer_FinalizedColor;

void main()
{
    // If we are already drawing skybox there is no need to sample below's textures so we just return skybox color
    if (fromVert_DrawingSkybox == 1)
    {
        toFramebuffer_FinalizedColor = texture(fromCode_Skybox, fromVert_UVW);
        return;
    }

    // A single vec3 UV coordinates are used for both the skybox UVs (vec3) and sampling UVs (vec2) so we just get XY
    const vec2 sampleUV = fromVert_UVW.xy;

    // Check if a pixel has been dran in current texel and if not discard it
    const float depth = texture(fromCode_DepthBuffer, sampleUV).r;
    if (PUSH_CONSTANT.renderedImageValue == RenderedImageValue_DepthBuffer) { toFramebuffer_FinalizedColor = vec4(depth, depth, depth, 1.0); return; }

    // Check if fragment we are writing to actually should contain a color
    if (depth >= 1.0f)
    {
        discard;
    }

    /*
        Load and store G-Buffer values
    */

    const vec3 position = GetWorldPositionFromDepth(sampleUV, depth, uniformBuffer.inverseProjection, uniformBuffer.inverseView);
    if (PUSH_CONSTANT.renderedImageValue == RenderedImageValue_PositionBuffer) { toFramebuffer_FinalizedColor = vec4(position, 1.0); return; }

    const vec3 diffuse = texture(fromCode_DiffuseBuffer, sampleUV).rgb;
    if (PUSH_CONSTANT.renderedImageValue == RenderedImageValue_DiffuseBuffer) { toFramebuffer_FinalizedColor = vec4(diffuse, 1.0); return; }

    const vec3 specularAndShininess = texture(toFramebuffer_SpecularAndShinines, sampleUV).rgb;
    if (PUSH_CONSTANT.renderedImageValue == RenderedImageValue_SpecularBuffer) { toFramebuffer_FinalizedColor = vec4(specularAndShininess.rrr, 1.0); return; }
    if (PUSH_CONSTANT.renderedImageValue == RenderedImageValue_ShininessBuffer) { toFramebuffer_FinalizedColor = vec4(specularAndShininess.ggg, 1.0); return; }

    const float specular = specularAndShininess.r;
    const float shininess = specularAndShininess.g;

    const vec3 normal = texture(fromCode_NormalBuffer, sampleUV).rgb;
    if (PUSH_CONSTANT.renderedImageValue == RenderedImageValue_NormalBuffer) { toFramebuffer_FinalizedColor = vec4(normal, 1.0); return; }

    vec4 shadowMapUV = PUSH_CONSTANT.lightSpaceMatrix * vec4(position, 1.0);

    float shadow = 1.0f;
    if (PUSH_CONSTANT.enableShadows == 1)
    {
        #if defined(SETTINGS_USE_PCF_SHADOWS)
            shadow = CalculateShadowPCF(fromCode_ShadowMap, shadowMapUV);
        #elif defined(SETTINGS_USE_POISSON_PCF_SHADOWS)
            shadow = CalculatePoissonShadowPCF(fromCode_ShadowMap, shadowMapUV, position);
        #elif defined(SETTINGS_USE_VARIANCE_SHADOWS)
            shadow = CalculateVarianceShadow(fromCode_ShadowMap, shadowMapUV, position);
        #elif defined(SETTINGS_USE_MOMENT_SHADOWS)
            shadow = CalculateMomentShadow(fromCode_ShadowMap, shadowMapUV, depth);
        #else
            shadow = CalculateShadow(fromCode_ShadowMap, shadowMapUV, vec2(0.0, 0.0));
        #endif
    }

    if (PUSH_CONSTANT.renderedImageValue == RenderedImageValue_ShadowBuffer) { toFramebuffer_FinalizedColor = vec4(shadow, shadow, shadow, 1.0); return; }

    // Define the final color
    vec3 calculatedColor = vec3(0, 0, 0);

    // For each directional light calculate its influence and add to the calculated color
    for (uint i = 0; i < storageBuffer.directionalLightCount; i++)
    {
        if (storageBuffer.directionalLights[i].intensity <= 0.0001f) continue;

        calculatedColor += CalculateDirectionalLight(
            storageBuffer.directionalLights[i],
            position,
            diffuse,
            specular,
            shininess * 512.0f,
            normal
        );
    }

    // For each point light calculate its influence and add to the calculated color
    for (uint i = 0; i < storageBuffer.pointLightCount; i++)
    {
        if (storageBuffer.pointLights[i].intensity <= 0.0001f) continue;

        calculatedColor += CalculatePointLight(
            storageBuffer.pointLights[i],
            position,
            diffuse,
            specular,
            shininess * 512.0f,
            normal
        );
    }

    // Submit data to framebuffer
    calculatedColor *= shadow;
    toFramebuffer_FinalizedColor = vec4(calculatedColor, 1.0);
}