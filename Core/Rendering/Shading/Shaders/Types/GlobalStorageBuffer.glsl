#ifndef GLOABL_STORAGE_BUFFER
#define GLOABL_STORAGE_BUFFER

#include "../Utility/ShaderDefinitions.glsl"
#include "../Lighting/DefaultLighting.glsl"

struct ObjectData
{
    mat4 model;
};

layout(std140, set = 0, binding = STORAGE_BUFFER_BINDING) readonly buffer StorageBuffer
{
    ObjectData objectDatas[MAX_MESHES];

    DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
    PointLight pointLights[MAX_POINT_LIGHTS];

    uint directionalLightCount;
    uint pointLightCount;
} storageBuffer;

#endif