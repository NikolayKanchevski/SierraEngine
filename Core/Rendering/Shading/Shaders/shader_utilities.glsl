#version 450

#define MAX_TEXTURES 128

#define TEXTURE_TYPE_DIFFUSE_OFFSET 0
#define TEXTURE_TYPE_SPECULAR_OFFSET 1
#define TOTAL_TEXTURE_TYPES_COUNT 2

/* --- STRUCTS --- */

struct Material
{
    vec3 diffuse;
    float shininess;

    vec3 specular;
    vec3 ambient;
};

struct ObjectData
{
    mat4 model;
};

/* --- FIELDS --- */

layout(set = 0, binding = 0) uniform UniformBuffer
{
    mat4 view;
    mat4 projection;
} uniformBuffer;

layout(std140, set = 0, binding = 1) readonly buffer StorageBuffer
{
    ObjectData objectDatas[];
} storageBuffer;

layout(push_constant) uniform PushConstant
{
    Material material;

    uint meshID;
    uint meshTexturesPresence;
} pushConstant;

/* --- METHODS --- */

bool IsBitSet(uint binaryValue, uint bitIndex)
{
    return (binaryValue & (1 << bitIndex)) > 0;
}