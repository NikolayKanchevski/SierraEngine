//
// Created by Nikolay Kanchevski on 11.10.22.
//

#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct VertexP
{
    glm::vec3 position;
};

struct VertexPNT
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 textureCoordinates;
};
