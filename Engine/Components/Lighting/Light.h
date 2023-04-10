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
        Vector3 color = { 1, 1, 1 };

        /// @brief Intensity of the light color.
        float intensity = 1.0f;

        /// @brief Returns the ID of the light within the storage buffer array.
        [[nodiscard]] uint GetID() const { return lightID; }

        // TODO: CACHING
        [[nodiscard]] inline Matrix4x4 GetViewSpaceMatrix()
        {
            Recalculate();
            return projectionView;
        }

    protected:
        uint lightID;
        Matrix4x4 projectionView;

        inline virtual void Recalculate()
        {

        }
    };

}
