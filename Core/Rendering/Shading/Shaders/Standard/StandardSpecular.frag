#version 450

#include "../Utility/ShaderDefinitions.glsl"
#include "../Utility/ShaderUtilities.glsl"

layout(location = 0) in vec3 fromVert_Position;
layout(location = 1) in vec3 fromVert_Normal;
layout(location = 2) in vec2 fromVert_TextureCoordinates;

layout(set = 1, binding = SPECULAR_TEXTURE_OFFSET + 2) uniform sampler2D specularSampler;

layout(location = 0) out vec4 outColor;

void main()
{
        // Read diffuse and specular textures
        vec3 textureColor = texture(specularSampler, fromVert_TextureCoordinates).rgb;

        // Submit the final color for drawing
        outColor = vec4(textureColor, 1.0);
}