//
// Created by Nikolay Kanchevski on 13.11.22.
//

#pragma once

#include "Component.h"

namespace Sierra::Engine
{

    /// @brief A component representing any type of light in the scene.
    class Light : public Component
    {
    public:
        /* --- PROPERTIES --- */
        /// @brief Color of the light.
        Vector3 color = { 1, 1, 1 };

        /// @brief Intensity of the light color.
        float intensity = 1.0f;

        /* --- CONSTRUCTORS --- */
        Light() = default;
        void OnAddComponent() override;

        /* --- POLLING METHODS --- */
        /// @brief Recalculates the view space matrix.
        inline virtual void Recalculate() { }

        /* --- GETTER METHODS --- */
        /// @brief Returns the ID of the light within the storage buffer array.
        [[nodiscard]] uint32 GetID() const { return lightID; }

        /// @brief Returns the matrix that is used to calculate what is in shadow by the light.
        [[nodiscard]] inline Matrix4x4 GetViewSpaceMatrix() { return projectionView; }

    protected:
        uint32 lightID;
        Matrix4x4 projectionView;

    };

}
