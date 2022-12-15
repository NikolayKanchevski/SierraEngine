//
// Created by Nikolay Kanchevski on 10.10.22.
//

#pragma once

#include "Light.h"

namespace Sierra::Engine::Components
{

    /// @brief A component class representing a directional light in the scene. Derives from both <see cref="Light"/> and <see cref="Component"/>.
    class DirectionalLight : public Light
    {
    public:
        /* --- PROPERTIES --- */
        /// @brief What direction the light casting will follow.
        glm::vec3 direction = { 0, -1, 0 };

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

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline static uint32_t GetDirectionalLightCount() { return directionalLightCount; }

        /* --- DESTRUCTOR --- */
        inline void Destroy() const override { RemoveComponent<DirectionalLight>(); freedIDs.push_back(this->lightID); directionalLightCount--; };

    public:
        struct alignas(16) ShaderDirectionalLight
        {
            glm::vec3 direction;
            float intensity;

            glm::vec3 color;
        };

        operator ShaderDirectionalLight() const noexcept { return
        {
            .direction = this->direction,
            .intensity = this->intensity,
            .color = this->color
        }; }

    private:
        inline static uint32_t currentMaxID = 0;

        inline static std::vector<uint32_t> freedIDs;
        inline static uint32_t directionalLightCount = 0;
    };

}