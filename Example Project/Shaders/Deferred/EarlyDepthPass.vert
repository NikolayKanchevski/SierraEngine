#version 450

/* !COMPILE_TO_BINARY */
#include "../Types/GlobalUniformBuffer.glsl"
#include "../Types/GlobalStorageBuffer.glsl"
#include "../Types/MeshPushConstant.glsl"

layout(location = 0) in vec3 fromCode_Position;
layout(location = 1) in vec3 fromCode_Normal;
layout(location = 2) in vec2 fromCode_UV;

const uint HEIGHT_TEXTURE_BINDING = 5;
layout(binding = HEIGHT_TEXTURE_BINDING) uniform sampler2D heightMapSampler;

void main()
{
    // Set the position of the vertex in world space
    vec4 vertexPosition = storageBuffer.objectData[pushConstant.meshID].modelMatrix * vec4(fromCode_Position, 1.0f);
    if (pushConstant.material.vertexExaggeration > 0.0) vertexPosition += vec4(pushConstant.material.vertexExaggeration * texture(heightMapSampler, fromCode_UV).r * fromCode_Normal, 0.0f);
    gl_Position = uniformBuffer.projection * uniformBuffer.view * vertexPosition;
}