//
// Created by Nikolay Kanchevski on 10.10.22.
//

#pragma once

#include <cstdint>

class World
{
public:
    static const uint32_t MAX_TEXTURES = 128; // Changed as @kael wouldn't stop bitching about it
    static const int MAX_POINT_LIGHTS = 64; // Remember to change the limit in the fragment shader too!
    static const int MAX_DIRECTIONAL_LIGHTS = 16; // Remember to change the limit in the fragment shader too!
    static const int MAX_SPOTLIGHT_LIGHTS = 16; // Remember to change the limit in the fragment shader too!

    /* --- CONSTRUCTORS --- */

    /* --- POLLING METHODS --- */

    /* --- SETTER METHODS --- */

    /* --- GETTER METHODS --- */

    /* --- DESTRUCTOR --- */
private:

};
