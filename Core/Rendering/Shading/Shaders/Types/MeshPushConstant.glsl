#ifndef MESH_PUSH_CONSTANT_GLSL
#define MESH_PUSH_CONSTANT_GLSL

#include "../Types/Material.glsl"

layout(push_constant) uniform MeshPushConstant
{
    Material material;

    uint entityID;
    uint meshID;
    uint meshTexturesPresence;
} pushConstant;

#endif