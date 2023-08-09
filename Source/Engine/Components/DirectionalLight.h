//
// Created by Nikolay Kanchevski on 10.10.22.
//

#pragma once

#include "Light.h"
#include "../Classes/IdentifierPool.h"
#include "../../Core/Rendering/RenderingSettings.h"

namespace Sierra::Engine
{

    /// @brief A component class representing a directional light in the scene. Derives from both <see cref="Light"/> and <see cref="Component"/>.
    class DirectionalLight : public Light
    {
    public:
        /* --- CONSTRUCTORS --- */
        DirectionalLight() = default;

        /* --- POLLING METHODS --- */
        void OnDrawUI() override;
        void Recalculate() override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline Vector3 GetDirection() const { return direction; }

    private:
        Vector3 direction = { 0.0f, 1.0f, 0.0f };

    };

}