#ifndef MATERIAL_GLSL
#define MATERIAL_GLSL

struct Material
{
    vec3 diffuse;
    float specular;

    float shininess;
    float vertexExaggeration;
};

#endif