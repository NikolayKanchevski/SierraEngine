//
// Created by Nikolay Kanchevski on 10.10.22.
//

#pragma once

#include "Light.h"
#include "../../Classes/IdentifierPool.h"
#include "../../../Core/Rendering/RenderingSettings.h"

namespace Sierra::Engine
{

    /// @brief A component class representing a directional light in the scene. Derives from both <see cref="Light"/> and <see cref="Component"/>.
    class DirectionalLight : public Light
    {
    public:
        /* --- CONSTRUCTORS --- */
        DirectionalLight();

        /* --- POLLING METHODS --- */
        void OnDrawUI() override;
        void Recalculate() override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline static uint GetDirectionalLightCount() { return IDPool.GetTotalIDsCount(); }

        /* --- DESTRUCTOR --- */
        void Destroy() override;

        /* --- OPERATORS --- */
        struct ShaderDirectionalLight
        {
            Matrix4x4 projectionView;

            Vector3 direction;
            float intensity;

            Vector3 color;
            float _align1_;
        };
        operator ShaderDirectionalLight() const;


    private:
        Vector3 direction = { 0.0f, 1.0f, 0.0f };
        inline static auto IDPool = IdentifierPool<uint>(MAX_DIRECTIONAL_LIGHTS);

    };

}