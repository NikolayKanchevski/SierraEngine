//
// Created by Nikolay Kanchevski on 10.10.22.
//

#pragma once


#include <glm/vec3.hpp>

class DirectionalLight
{
public:
    struct alignas(16) UniformDirectionalLight
    {
        glm::vec3 direction;
        float intensity;

        glm::vec3 color;
    };

    /* --- CONSTRUCTORS --- */

    /* --- POLLING METHODS --- */

    /* --- SETTER METHODS --- */

    /* --- GETTER METHODS --- */

    /* --- DESTRUCTOR --- */
private:

};
