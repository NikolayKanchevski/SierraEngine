#version 450

#include "shader_utilities.glsl"

layout(location = 0) in vec3 fromVert_Position;
layout(location = 1) in vec3 fromVert_Normal;
layout(location = 2) in vec2 fromVert_TextureCoordinates;

layout(set = 1, binding = 2) uniform sampler2D texturePool[MAX_TEXTURES];

layout(location = 0) out vec4 outColor;

void main()
{
    vec3 diffuseTextureColor;

    if (IsBitSet(pushConstant.meshTexturesPresence, TEXTURE_TYPE_DIFFUSE_OFFSET)) diffuseTextureColor = texture(texturePool[(pushConstant.meshID * TOTAL_TEXTURE_TYPES_COUNT) + TEXTURE_TYPE_DIFFUSE_OFFSET], fromVert_TextureCoordinates).rgb;
    else diffuseTextureColor = texture(texturePool[TEXTURE_TYPE_DIFFUSE_OFFSET], fromVert_TextureCoordinates).rgb;

    outColor = vec4(diffuseTextureColor, 1.0);
}
