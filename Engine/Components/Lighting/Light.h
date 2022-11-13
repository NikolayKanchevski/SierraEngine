//
// Created by Nikolay Kanchevski on 13.11.22.
//

#pragma once

#include "../Component.h"

namespace Sierra::Engine::Components
{

    /// @brief A component representing any type of light in the scene.
    class Light : public Component
    {
    public:
        /// @brief Color of the light.
        glm::vec3 color = { 1, 1, 1 };

        /// @brief Intensity of the light color.
        float intensity = 1.0f;

        /// @brief Returns the ID of the light within the storage buffer array.
        [[nodiscard]] uint32_t GetID() const { return lightID; }

    protected:
        uint32_t lightID;
    };

}
