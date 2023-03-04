#version 450

#include "../Lighting/DefaultLighting.glsl"
#include "../Types/GlobalStorageBuffer.glsl"

layout(location = 0) in vec2 fromVert_UV;

layout(set = 0, binding = 2) uniform sampler2D fromCode_PositionBuffer;
layout(set = 0, binding = 3) uniform sampler2D fromCode_DiffuseBuffer;
layout(set = 0, binding = 4) uniform sampler2D fromCode_SpecularAndShininess;
layout(set = 0, binding = 5) uniform sampler2D fromCode_NormalBuffer;
layout(set = 0, binding = 6) uniform sampler2D fromCode_DepthBuffer;

const uint RenderedImageValue_RenderedImage = 0;
const uint RenderedImageValue_PositionBuffer = 1;
const uint RenderedImageValue_DiffuseBuffer = 2;
const uint RenderedImageValue_SpecularBuffer = 3;
const uint RenderedImageValue_ShininessBuffer = 4;
const uint RenderedImageValue_NormalBuffer = 5;
const uint RenderedImageValue_DepthBuffer = 6;

layout(push_constant) uniform FinalizationPushConstant
{
    uint renderedImageValue;
} pushConstant;

layout (location = 0) out vec4 toFramebuffer_FinalizedColor;

void main()
{
    // Check if a pixel has been dran in current texel and if not discard it
    float depth = texture(fromCode_DepthBuffer, fromVert_UV).r;
    if (depth == 1.0)
    {
        toFramebuffer_FinalizedColor = texture(fromCode_DiffuseBuffer, fromVert_UV);
        return;
    }
    if (pushConstant.renderedImageValue == RenderedImageValue_DepthBuffer) { toFramebuffer_FinalizedColor = vec4(depth, depth, depth, 1.0); return; }

    // Load values and return depending on rendered output type
    vec3 position = texture(fromCode_PositionBuffer, fromVert_UV).xyz;
    if (pushConstant.renderedImageValue == RenderedImageValue_PositionBuffer) { toFramebuffer_FinalizedColor = vec4(position, 1.0); return; }

    vec3 diffuse = texture(fromCode_DiffuseBuffer, fromVert_UV).rgb;
    if (pushConstant.renderedImageValue == RenderedImageValue_DiffuseBuffer) { toFramebuffer_FinalizedColor = vec4(diffuse, 1.0); return; }

    vec2 specularAndShininess = texture(fromCode_SpecularAndShininess, fromVert_UV).rg;
    if (pushConstant.renderedImageValue == RenderedImageValue_SpecularBuffer) { toFramebuffer_FinalizedColor = vec4(specularAndShininess.xxx, 1.0); return; }
    if (pushConstant.renderedImageValue == RenderedImageValue_ShininessBuffer) { toFramebuffer_FinalizedColor = vec4(specularAndShininess.y / 512.0f, specularAndShininess.y / 512.0f, specularAndShininess.y / 512.0f, 1.0); return; }

    float specular = specularAndShininess.x;
    float shininess = specularAndShininess.y;

    vec3 normal = texture(fromCode_NormalBuffer, fromVert_UV).rgb;
    if (pushConstant.renderedImageValue == RenderedImageValue_NormalBuffer) { toFramebuffer_FinalizedColor = vec4(normal, 1.0); return; }

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
            shininess,
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
            shininess,
            normal
        );
    }

    // Submit data to framebuffer
    toFramebuffer_FinalizedColor = vec4(calculatedColor, 1.0);
}