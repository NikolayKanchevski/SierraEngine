//
// Created by Nikolay Kanchevski on 10.10.22.
//

#pragma once

#include "Light.h"

#include "../Classes/IdentifierPool.h"
#include "../../Core/Rendering/RenderingSettings.h"

namespace Sierra::Engine
{

    /// @brief A component class representing a point32 light in the scene. Derives from both <see cref="Light"/> and <see cref="Component"/>.
    class PointLight : public Light
    {
    public:
        /* --- PROPERTIES --- */
        /// @brief Linear value of the light. See <a href="https://learnopengl.com/Lighting/Light-casters">this link</a>
        /// and scroll down to Point32 Lights if you are not familiar with what this is used for.
        float linear = 0.09f;

        /// @brief Quadratic value of the light. See <a href="https://learnopengl.com/Lighting/Light-casters">this link</a>
        /// and scroll down to Point32 Lights if you are not familiar with what this is used for.
        float quadratic = 0.032f;

        /* --- CONSTRUCTORS --- */
        PointLight() = default;

        /* --- POLLING METHODS --- */
        void OnDrawUI() override;

    private:
        inline void Recalculate() override { }

    };

}
