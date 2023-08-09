#ifndef MESH_PUSH_CONSTANT_GLSL
#define MESH_PUSH_CONSTANT_GLSL

struct Material
{
    vec3 diffuse;
    float specular;

    float shininess;
    float vertexExaggeration;
};

layout(push_constant) uniform PushConstant
{
    Material material;

    uint meshID;
    uint entityID;
    uint meshTexturesPresence;
    float _align1_;
} pushConstant;

#endif