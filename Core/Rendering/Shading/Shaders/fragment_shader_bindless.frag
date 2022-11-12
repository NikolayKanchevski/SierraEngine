#version 450

#define MAX_TEXTURES 128

#define TEXTURE_TYPE_DIFFUSE_OFFSET 0
#define TEXTURE_TYPE_SPECULAR_OFFSET 1
#define TOTAL_TEXTURE_TYPES_COUNT 2

layout(location = 0) in vec3 fromVert_Position;
layout(location = 1) in vec3 fromVert_Normal;
layout(location = 2) in vec2 fromVert_TextureCoordinates;

struct Material
{
    vec3 diffuse;
    float shininess;

    vec3 specular;
    vec3 ambient;
};

layout(set = 0, binding = 0) uniform UniformBuffer
{
    mat4 view;
    mat4 projection;
} uniformBuffer;

layout(push_constant) uniform PushConstant
{
    Material material;

    uint meshID;
    uint meshTexturesPresence;
} pushConstant;

layout(set = 1, binding = 2) uniform sampler2D texturePool[MAX_TEXTURES];

layout(location = 0) out vec4 outColor;

bool IsBitSet(uint binaryValue, uint bitIndex);

void main()
{
    vec3 diffuseTextureColor;

    if (IsBitSet(pushConstant.meshTexturesPresence, TEXTURE_TYPE_DIFFUSE_OFFSET)) diffuseTextureColor = texture(texturePool[(pushConstant.meshID * TOTAL_TEXTURE_TYPES_COUNT) + TEXTURE_TYPE_DIFFUSE_OFFSET], fromVert_TextureCoordinates).rgb;
    diffuseTextureColor = texture(texturePool[TEXTURE_TYPE_DIFFUSE_OFFSET], fromVert_TextureCoordinates).rgb;

    outColor = vec4(diffuseTextureColor, 1.0);
}

bool IsBitSet(uint binaryValue, uint bitIndex)
{
    return (binaryValue & (1 << bitIndex)) > 0;
}
