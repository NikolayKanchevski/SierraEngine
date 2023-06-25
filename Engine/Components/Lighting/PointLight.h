//
// Created by Nikolay Kanchevski on 10.10.22.
//

#pragma once

#include "Light.h"

#include "../Transform.h"
#include "../../Classes/IdentifierPool.h"
#include "../../../Core/Rendering/UI/ImGuiUtilities.h"

namespace Sierra::Engine::Components
{

    /// @brief A component class representing a point light in the scene. Derives from both <see cref="Light"/> and <see cref="Component"/>.
    class PointLight : public Light
    {
    public:
        /* --- PROPERTIES --- */
        /// \brief Linear value of the light. See <a href="https://learnopengl.com/Lighting/Light-casters">this link</a>
        /// and scroll down to Point Lights if you are not familiar with what this is used for.
        float linear = 0.09f;

        /// \brief Quadratic value of the light. See <a href="https://learnopengl.com/Lighting/Light-casters">this link</a>
        /// and scroll down to Point Lights if you are not familiar with what this is used for.
        float quadratic = 0.032f;

        /* --- CONSTRUCTORS --- */
        inline PointLight()
        {
            lightID = IDPool.CreateNewID();
        }

        /* --- POLLING METHODS --- */
        inline void OnDrawUI() override
        {
            GUI::BeginProperties();

            GUI::FloatProperty("Intensity:", intensity);

            static const float resetValues[3] = { 0.0f, 0.0f, 0.0f };
            static const char* tooltips[3] = { "Some tooltip.", "Some tooltip.", "Some tooltip." };
            GUI::PropertyVector3("Color:", color, resetValues, tooltips);

            GUI::FloatProperty("Linear:", linear);
            GUI::FloatProperty("Quadratic:", quadratic);


            GUI::EndProperties();
        }

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline static uint GetPointLightCount() { return IDPool.GetTotalIDsCount(); }

        /* --- DESTRUCTOR --- */
        inline void Destroy() override { IDPool.RemoveID(lightID); };

    public:
        struct ShaderPointLight
        {
            Matrix4x4 projectionView;

            Vector3 color;
            float intensity;

            Vector3 position;
            float linear;

            float quadratic;
            Vector3 _align_1;
        };

        operator ShaderPointLight() const noexcept { auto position = GetComponent<Transform>().GetWorldPosition(); return
        {
            .projectionView = projectionView,
            .color = color,
            .intensity = intensity,
            .position = { position.x, -position.y, position.z },
            .linear = linear,
            .quadratic = quadratic
        }; }

    private:
        inline void Recalculate() override
        {

        }

        inline static auto IDPool = Classes::IdentifierPool<uint>(MAX_POINT_LIGHTS);
    };

}
