//
// Created by Nikolay Kanchevski on 11.10.22.
//

#pragma once

#include <glm/vec3.hpp>

namespace Sierra::Engine::Components
{

    class Transform
    {
    public:
        /* --- PROPERTIES --- */
        glm::vec3 position = {0, 0, 0 };
        glm::vec3 rotation = { 0, 0, 0 };
        glm::vec3 scale = { 1, 1, 1 };

    private:

    };

}
