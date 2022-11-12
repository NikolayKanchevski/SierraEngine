#version 450

#define TOTAL_TEXTURE_TYPES_COUNT 2

layout(location = 0) in vec3 fromCode_Position;
layout(location = 1) in vec3 fromCode_Normal;
layout(location = 2) in vec2 fromCode_TextureCoordinates;

layout(set = 0, binding = 0) uniform UniformBuffer
{
    mat4 view;
    mat4 projection;
} uniformBuffer;

struct ObjectData
{
    mat4 model;
};

layout(std140, set = 0, binding = 1) readonly buffer StorageBuffer
{
    ObjectData objectDatas[];
} storageBuffer;

struct Material
{
    vec3 diffuse;
    float shininess;

    vec3 specular;
    vec3 ambient;
};

layout(push_constant) uniform PushConstant
{
    Material material;

    uint meshID;
    uint meshTexturesPresence;
} pushConstant;

layout(location = 0) out vec3 toFrag_Position;
layout(location = 1) out vec3 toFrag_Normal;
layout(location = 2) out vec2 toFrag_TextureCoordinates;

void main()
{
    // Set the position of the vertex in world space
    vec4 vertexPosition = storageBuffer.objectDatas[pushConstant.meshID].model * vec4(fromCode_Position, 1.0);
    gl_Position = uniformBuffer.projection * uniformBuffer.view * vertexPosition;

    // Calculate normal matrix
    mat3 normalMatrix = transpose(inverse(mat3(storageBuffer.objectDatas[pushConstant.meshID].model)));

    // Transfer required data from vertex to fragment shader
    toFrag_Position = vertexPosition.xyz;
    toFrag_Normal = normalize(normalMatrix * fromCode_Normal);
    toFrag_TextureCoordinates = fromCode_TextureCoordinates;
}