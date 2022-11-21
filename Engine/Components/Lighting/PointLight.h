//
// Created by Nikolay Kanchevski on 10.10.22.
//

#pragma once

#include "Light.h"
#include "../InternalComponents.h"

namespace Sierra::Engine::Components {

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
            pointLightCount++;

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
        }

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline static uint32_t GetPointLightCount() { return pointLightCount; }

        /* --- DESTRUCTOR --- */
        inline void Destroy() const override { RemoveComponent<PointLight>(); freedIDs.push_back(this->lightID); pointLightCount--; };

    public:
        struct ShaderPointLight
        {
            glm::vec3 position;
            float linear;

            glm::vec3 color;
            float intensity;

            float quadratic;
            glm::vec3 _align_1;
        };

        operator ShaderPointLight() const noexcept { auto position = GetComponent<Transform>().position; return
        {
            .position = { position.x, -position.y, position.z },
            .linear = this->linear,
            .color = this->color,
            .intensity = this->intensity,
            .quadratic = this->quadratic
        }; }

    private:
        inline static uint32_t currentMaxID = 0;

        inline static std::vector<uint32_t> freedIDs;
        inline static uint32_t pointLightCount = 0;
    };

}
