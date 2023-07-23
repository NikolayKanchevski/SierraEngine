#version 450

/* !COMPILE_TO_BINARY */
#define SETTINGS_SHADING_MODEL_BLINN_PHONG

#include "../Utility/Depth.glsl"
#include "../Utility/NormalMapping.glsl"
#include "../Lighting/DefaultLighting.glsl"
#include "../Utility/ShaderDefinitions.glsl"
#include "../Types/GlobalUniformBuffer.glsl"
#include "../Types/GlobalStorageBuffer.glsl"

const uint RendererOutputValue_RenderedImage = 0;
const uint RendererOutputValue_PositionBuffer = 1;
const uint RendererOutputValue_DiffuseBuffer = 2;
const uint RendererOutputValue_SpecularBuffer = 3;
const uint RendererOutputValue_ShininessBuffer = 4;
const uint RendererOutputValue_NormalBuffer = 5;
const uint RendererOutputValue_DepthBuffer = 6;
layout(constant_id = 0) const uint RENDERER_OUTPUT = RendererOutputValue_RenderedImage;

layout(location = 0) in flat uint fromVert_DrawingSkybox;
layout(location = 1) in vec3 fromVert_UVW;

const uint DIFFUSE_BUFFER_BINDING   =   2;
const uint SS_BUFFER_BINDING        =   3;          // Specular & Shininess
const uint NORMAL_BUFFER_BINDING    =   4;
const uint DEPTH_BUFFER_BINDING     =   5;
const uint SKYBOX_BUFFER_BINDING    =   6;
layout(input_attachment_index = 0, binding = DIFFUSE_BUFFER_BINDING) uniform subpassInput fromSubpass_DiffuseBuffer;
layout(input_attachment_index = 1, binding = SS_BUFFER_BINDING) uniform subpassInput fromSubpass_SpecularAndShininess;
layout(input_attachment_index = 2, binding = NORMAL_BUFFER_BINDING) uniform usubpassInput fromSubpass_NormalBuffer;
layout(input_attachment_index = 3, binding = DEPTH_BUFFER_BINDING) uniform subpassInput fromSubpass_DepthBuffer;
layout(binding = SKYBOX_BUFFER_BINDING) uniform samplerCube fromCode_Skybox;

layout(location = 0) out vec4 toFramebuffer_FinalizedColor;

void main()
{
    // Get depth from G-Buffer to make a manual "depth test"
    const float depth = subpassLoad(fromSubpass_DepthBuffer).r;

    // If we are already drawing skybox there is no need to sample below's textures so we just return skybox color
    if (fromVert_DrawingSkybox == 1)
    {
        // Check if fragment we are writing to actually should contain a color
        if (depth < 1.0f)
        {
            discard;
        }

        // Otherwise asign skybox color
        toFramebuffer_FinalizedColor = texture(fromCode_Skybox, fromVert_UVW);
        return;
    }

    // A single vec3 UV coordinates are used for both the skybox UVs (vec3) and sampling UVs (vec2) so we just get XY

    /*
        Load and store G-Buffer values
    */
    if (RENDERER_OUTPUT == RendererOutputValue_DepthBuffer)
    {
        float linearizedDepth = LinearizeDepth(depth, uniformBuffer.nearClip, uniformBuffer.farClip);
        toFramebuffer_FinalizedColor = vec4(vec3(linearizedDepth), 1.0);
        return;
    }

    const vec2 sampleUV = fromVert_UVW.xy;
    const vec3 position = GetWorldPositionFromDepth(sampleUV, depth, uniformBuffer.inverseProjection, uniformBuffer.inverseView);
    if (RENDERER_OUTPUT == RendererOutputValue_PositionBuffer) { toFramebuffer_FinalizedColor = vec4(position, 1.0); return; }

    const vec3 diffuse = subpassLoad(fromSubpass_DiffuseBuffer).rgb;
    if (RENDERER_OUTPUT == RendererOutputValue_DiffuseBuffer) { toFramebuffer_FinalizedColor = vec4(diffuse, 1.0); return; }

    const vec2 specularAndShininess = subpassLoad(fromSubpass_SpecularAndShininess).rg;
    if (RENDERER_OUTPUT == RendererOutputValue_SpecularBuffer) { toFramebuffer_FinalizedColor = vec4(specularAndShininess.rrr, 1.0); return; }
    if (RENDERER_OUTPUT == RendererOutputValue_ShininessBuffer) { toFramebuffer_FinalizedColor = vec4(specularAndShininess.ggg, 1.0); return; }

    const float specular = specularAndShininess.r;
    const float shininess = specularAndShininess.g;

    const vec3 normal = DecompressNormal32(subpassLoad(fromSubpass_NormalBuffer).r);
    if (RENDERER_OUTPUT == RendererOutputValue_NormalBuffer) { toFramebuffer_FinalizedColor = vec4(normal, 1.0); return; }

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
    toFramebuffer_FinalizedColor = vec4(calculatedColor, 1.0);
}