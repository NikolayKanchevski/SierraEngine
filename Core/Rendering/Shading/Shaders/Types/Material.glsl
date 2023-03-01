#ifndef MATERIAL
#define MATERIAL

struct Material
{
    vec3 diffuse;
    float specular;

    float shininess;
    float ambient;

    float vertexExaggeration;
};

#endif