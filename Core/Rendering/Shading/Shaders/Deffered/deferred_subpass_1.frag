#version 450

#include "../shader_utilities.glsl"

layout(location = 0) in vec2 fromVert_UV;

layout(input_attachment_index = 0, set = 1, binding = 2) uniform subpassInput fromSubpass_PositionBuffer;
layout(input_attachment_index = 1, set = 1, binding = 3) uniform subpassInput fromSubpass_ColorBuffer;
layout(input_attachment_index = 2, set = 1, binding = 4) uniform subpassInput fromSubpass_SpecularAndShininess;
layout(input_attachment_index = 3, set = 1, binding = 5) uniform subpassInput fromSubpass_NormalBuffer;
layout(input_attachment_index = 4, set = 1, binding = 6) uniform subpassInput fromSubpass_DepthBuffer;

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
    float depth = subpassLoad(fromSubpass_DepthBuffer).r;
    if (depth == 1.0)
    {
        discard;
    }
    if (pushConstant.renderedImageValue == RenderedImageValue_DepthBuffer) { toFramebuffer_FinalizedColor = vec4(depth, depth, depth, 1.0); return; }


    // Load values and return depending on rendered output type
    vec3 position = subpassLoad(fromSubpass_PositionBuffer).xyz;
    if (pushConstant.renderedImageValue == RenderedImageValue_PositionBuffer) { toFramebuffer_FinalizedColor = vec4(position, 1.0); return; }

    vec3 color = subpassLoad(fromSubpass_ColorBuffer).rgb;
    if (pushConstant.renderedImageValue == RenderedImageValue_DiffuseBuffer) { toFramebuffer_FinalizedColor = vec4(color, 1.0); return; }

    vec2 specularAndShininess = subpassLoad(fromSubpass_SpecularAndShininess).rg;
    if (pushConstant.renderedImageValue == RenderedImageValue_SpecularBuffer) { toFramebuffer_FinalizedColor = vec4(specularAndShininess.xxx, 1.0); return; }
    if (pushConstant.renderedImageValue == RenderedImageValue_ShininessBuffer) { toFramebuffer_FinalizedColor = vec4(specularAndShininess.yyy, 1.0); return; }

    float specular = specularAndShininess.x;
    float shininess = specularAndShininess.y;

    vec3 normal = subpassLoad(fromSubpass_NormalBuffer).rgb;
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
            color,
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
            color,
            specular,
            shininess,
            normal
        );
    }

    // Submit data to framebuffer
    toFramebuffer_FinalizedColor = vec4(calculatedColor, 1.0);
}