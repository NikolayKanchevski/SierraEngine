//
// Created by Nikolay Kanchevski on 10.10.22.
//

#pragma once

#include "Light.h"
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
            directionalLightCount++;

            if (freedIDs.empty())
            {
                this->lightID = currentMaxID;
                currentMaxID++;
            }
            else
            {
                this->lightID = freedIDs[0];
                freedIDs.erase(freedIDs.begin());
            }
        };

        /* --- POLLING METHODS --- */
        inline void DrawUI() override
        {
            ImGui::BeginProperties();

            ImGui::FloatProperty("Intensity:", intensity);

            static const float resetValues[3] = { 0.0f, 0.0f, 0.0f };
            static const char* tooltips[3] = { "Some tooltip.", "Some tooltip.", "Some tooltip." };
            ImGui::PropertyVector3("Color:", color, resetValues, tooltips);

            ImGui::PropertyVector3("Direction:", direction, resetValues, tooltips);


            ImGui::EndProperties();
        }

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline static uint GetDirectionalLightCount() { return directionalLightCount; }

        /* --- DESTRUCTOR --- */
        inline void Destroy() const override { RemoveComponent<DirectionalLight>(); freedIDs.push_back(this->lightID); directionalLightCount--; };

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
        inline static uint currentMaxID = 0;

        inline static std::vector<uint> freedIDs;
        inline static uint directionalLightCount = 0;
    };

}