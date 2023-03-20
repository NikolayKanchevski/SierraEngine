//
// Created by Nikolay Kanchevski on 10.10.22.
//

#pragma once

#include "Light.h"
#include "../../Classes/IdentifierPool.h"
#include "../../../Core/Rendering/UI/ImGuiUtilities.h"

namespace Sierra::Engine::Components
{

    /// @brief A component class representing a directional light in the scene. Derives from both <see cref="Light"/> and <see cref="Component"/>.
    class DirectionalLight : public Light
    {
    public:
        /* --- PROPERTIES --- */
        /// @brief What direction the light casting will follow.
        Vector3 direction = { 0, -1, 0 };

        /* --- CONSTRUCTORS --- */
        inline DirectionalLight()
        {
            this->lightID = IDPool.CreateNewID();
        };

        /* --- POLLING METHODS --- */
        inline void OnDrawUI() override
        {
            GUI::BeginProperties();

            GUI::FloatProperty("Intensity:", intensity);

            static const float resetValues[3] = { 0.0f, 0.0f, 0.0f };
            static const char* tooltips[3] = { "Some tooltip.", "Some tooltip.", "Some tooltip." };
            GUI::PropertyVector3("Color:", color, resetValues, tooltips);

            GUI::PropertyVector3("Direction:", direction, resetValues, tooltips);


            GUI::EndProperties();
        }

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline static uint GetDirectionalLightCount() { return IDPool.GetTotalIDsCount(); }

        /* --- DESTRUCTOR --- */
        inline void Destroy() override { IDPool.RemoveID(lightID); };

    public:
        struct alignas(16) ShaderDirectionalLight
        {
            Vector3 direction;
            float intensity;

            Vector3 color;
        };

        operator ShaderDirectionalLight() const noexcept { return
        {
            .direction = this->direction,
            .intensity = this->intensity,
            .color = this->color
        }; }

    private:
        inline static auto IDPool = Classes::IdentifierPool<uint>(MAX_DIRECTIONAL_LIGHTS);
    };

}