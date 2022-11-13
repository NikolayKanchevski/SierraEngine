//
// Created by Nikolay Kanchevski on 11.10.22.
//

#pragma once

#include <glm/vec3.hpp>

struct Material
{
    glm::vec3 diffuse;
    float shininess;

    glm::vec3 specular;
    float _align1_;

    glm::vec3 ambient;
    float _align2_;
};