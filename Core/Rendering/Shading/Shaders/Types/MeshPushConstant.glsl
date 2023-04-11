#ifndef MESH_PUSH_CONSTANT_GLSL
#define MESH_PUSH_CONSTANT_GLSL

#include "../Types/Material.glsl"

layout(push_constant) uniform MeshPushConstant
{
    Material material;

    uint meshID;
    uint entityID;
    uint meshTexturesPresence;
    uint directionalLightID;
} pushConstant;

#endif