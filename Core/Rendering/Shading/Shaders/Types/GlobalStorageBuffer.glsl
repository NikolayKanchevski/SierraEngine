#ifndef GLOABL_STORAGE_BUFFER_GLSL
#define GLOABL_STORAGE_BUFFER_GLSL

#include "../Utility/ShaderDefinitions.glsl"
#include "../Lighting/DefaultLighting.glsl"

struct ObjectData
{
    mat4x4 modelMatrix;
    mat4x4 normalMatrix;
};

layout(std140, set = 0, binding = STORAGE_BUFFER_BINDING) readonly buffer StorageBuffer
{
    ObjectData objectDatas[MAX_MESHES];

    DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
    PointLight pointLights[MAX_POINT_LIGHTS];

    uint directionalLightCount;
    uint pointLightCount;

    vec2 _align1_;
} storageBuffer;

#endif