#version 450

#include "shader_utilities.glsl"

layout(location = 0) in vec3 fromVert_Position;
layout(location = 1) in vec3 fromVert_Normal;
layout(location = 2) in vec2 fromVert_TextureCoordinates;

layout(set = 1, binding = 2) uniform sampler2D diffuseSampler;
layout(set = 1, binding = 3) uniform sampler2D specularSampler;

layout(location = 0) out vec4 outColor;

void main()
{
        // Read diffuse and specular textures
        vec3 diffuseTextureColor = texture(diffuseSampler, fromVert_TextureCoordinates).rgb;
        float specularTextureColor = texture(specularSampler, fromVert_TextureCoordinates).r;

        // Calculate view direction
        vec3 viewDirection = normalize(-fromVert_Position);

        // Define the final color
        vec3 calculatedColor = vec3(0, 0, 0);

        // For each directional light calculate its influence and add to the calculated color
        for (uint i = 0; i < uniformBuffer.directionalLightCount; i++)
        {
                if (storageBuffer.directionalLights[i].intensity <= 0.0001) continue;

                calculatedColor += CalculateDirectionalLight(
                        storageBuffer.directionalLights[i],
                        fromVert_Normal,
                        viewDirection,
                        diffuseTextureColor,
                        specularTextureColor
                );
        }

        // For each point light calculate its influence and add to the calculated color
        for (int i = 0; i < uniformBuffer.pointLightCount; i++)
        {
                if (storageBuffer.pointLights[i].intensity <= 0.0001) continue;

                calculatedColor += CalculatePointLight(
                        storageBuffer.pointLights[i],
                        fromVert_Position,
                        fromVert_Normal,
                        viewDirection,
                        diffuseTextureColor,
                        specularTextureColor
                );
        }

        // Submit the final color for drawing
        outColor = vec4(calculatedColor, 1.0);
}