//
// Created by Nikolay Kanchevski on 10.10.22.
//

#pragma once


#include <glm/vec3.hpp>

class Spotlight
{
public:
    struct alignas(16) UniformSpotLight
    {
        glm::vec3 position;
        float radius;

        glm::vec3 direction;
        float intensity;

        glm::vec3 color;
        float linear;

        float quadratic;
        float spreadRadius;
    };

    /* --- CONSTRUCTORS --- */

    /* --- POLLING METHODS --- */

    /* --- SETTER METHODS --- */

    /* --- GETTER METHODS --- */

    /* --- DESTRUCTOR --- */
private:

};
