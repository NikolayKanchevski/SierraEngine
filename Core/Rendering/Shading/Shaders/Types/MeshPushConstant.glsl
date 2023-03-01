#ifndef MESH_PUSH_CONSTANT
#define MESH_PUSH_CONSTANT

#include "../Types/Material.glsl"

layout(push_constant) uniform MeshPushConstant
{
    Material material;

    uint entityID;
    uint meshID;
    uint meshTexturesPresence;
} pushConstant;

#endif