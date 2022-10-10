//
// Created by Nikolay Kanchevski on 10.10.22.
//

#pragma once


#include <glm/vec3.hpp>

class PointLight
{
    
public:
    struct alignas(16) UniformPointLight
    {
        glm::vec3 position;
        float linear;

        glm::vec3 color;
        float intensity;
    
        float quadratic;
    };

    /* --- CONSTRUCTORS --- */

    /* --- POLLING METHODS --- */

    /* --- SETTER METHODS --- */

    /* --- GETTER METHODS --- */

    /* --- DESTRUCTOR --- */
private:

};
