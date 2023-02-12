#version 450

#include "../shader_utilities.glsl"

layout(location = 0) in vec3 fromVert_Position;
layout(location = 1) in vec3 fromVert_Normal;
layout(location = 2) in vec2 fromVert_TextureCoordinates;

layout(push_constant) uniform MeshPushConstant
{
        Material material;

        uint meshID;
        uint meshTexturesPresence;
} pushConstant;

layout(set = 1, binding = DIFFUSE_TEXTURE_BINDING) uniform sampler2D diffuseSampler;
layout(set = 1, binding = SPECULAR_TEXTURE_BINDING) uniform sampler2D specularSampler;
layout(set = 1, binding = NORMAL_MAP_TEXTURE_BINDING) uniform sampler2D normalSampler;

layout(location = 0) out vec4 outColor;

void main()
{
        // Read diffuse and specular textures
        vec3 diffuseTextureColor = texture(diffuseSampler, fromVert_TextureCoordinates).rgb;
        float specularTextureColor = texture(specularSampler, fromVert_TextureCoordinates).r;
        vec3 normalTextureColor = texture(normalSampler, fromVert_TextureCoordinates).rgb;

        // Calculate view direction
        const vec3 viewDirection = normalize(-fromVert_Position);

        // Define the final color
        vec3 calculatedColor = vec3(0, 0, 0);

        // For each directional light calculate its influence and add to the calculated color
        for (uint i = 0; i < storageBuffer.directionalLightCount; i++)
        {
                if (storageBuffer.directionalLights[i].intensity <= 0.0001) continue;

                calculatedColor += CalculateDirectionalLight(
                        storageBuffer.directionalLights[i],
                        fromVert_Position,
                        diffuseTextureColor,
                        specularTextureColor,
                        pushConstant.material.shininess,
                        fromVert_Normal
                );
        }

        // For each point light calculate its influence and add to the calculated color
        for (uint i = 0; i < storageBuffer.pointLightCount; i++)
        {
                if (storageBuffer.pointLights[i].intensity <= 0.0001) continue;

                calculatedColor += CalculatePointLight(
                        storageBuffer.pointLights[i],
                        fromVert_Position,
                        diffuseTextureColor,
                        specularTextureColor,
                        pushConstant.material.shininess,
                        fromVert_Normal
                );
        }

        // Submit the final color for drawing
        outColor = vec4(calculatedColor, 1.0);
}