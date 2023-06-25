#version 450

/* !COMPILE_TO_BINARY */
#include "../Types/GlobalUniformBuffer.glsl"
#include "../Types/GlobalStorageBuffer.glsl"
#include "../Types/MeshPushConstant.glsl"
#include "../Utility/ShaderDefinitions.glsl"

layout(location = 0) in vec3 fromCode_Position;
layout(location = 1) in vec3 fromCode_Normal;
layout(location = 2) in vec2 fromCode_UV;

layout(binding = HEIGHT_TEXTURE_BINDING) uniform sampler2D heightMapSampler;

layout(location = 0) out vec3 toFrag_Position;
layout(location = 1) out vec3 toFrag_Normal;
layout(location = 2) out vec2 toFrag_UV;
layout(location = 3) out vec4 toFrag_ShadowUV;

void main()
{
    // Set the position of the vertex in world space
    vec4 vertexPosition = storageBuffer.objectDatas[pushConstant.meshID].model * vec4(fromCode_Position, 1.0f);
    vertexPosition += vec4(pushConstant.material.vertexExaggeration * texture(heightMapSampler, fromCode_UV).r * fromCode_Normal, 0.0f);
    gl_Position = uniformBuffer.projection * uniformBuffer.view * vertexPosition;

    // Calculate normal matrix
    mat3 normalMatrix = transpose(inverse(mat3(storageBuffer.objectDatas[pushConstant.meshID].model)));

    // Pass data to fragment shader
    toFrag_Position = vertexPosition.xyz;
    toFrag_Normal = normalize(normalMatrix * fromCode_Normal);
    toFrag_UV = fromCode_UV;
}