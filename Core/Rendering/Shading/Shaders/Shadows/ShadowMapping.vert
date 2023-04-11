#version 450

#include "../Utility/ShaderDefinitions.glsl"
#include "../Types/GlobalStorageBuffer.glsl"

layout(location = 0) in vec3 fromCode_Position;
layout(location = 1) in vec3 fromCode_Normal;
layout(location = 2) in vec2 fromCode_UV;

struct PushConstant
{
    uint meshID;
    uint directionalLightID;
};

SET_PUSH_CONSTANT(PushConstant);

void main()
{
    gl_Position = storageBuffer.directionalLights[PUSH_CONSTANT.directionalLightID].projectionView * storageBuffer.objectDatas[PUSH_CONSTANT.meshID].model * vec4(fromCode_Position, 1.0f);
}