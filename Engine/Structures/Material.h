//
// Created by Nikolay Kanchevski on 11.10.22.
//

#pragma once

#include <glm/vec3.hpp>

struct alignas(16) Material
{
    float shininess;

    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 ambient;
};