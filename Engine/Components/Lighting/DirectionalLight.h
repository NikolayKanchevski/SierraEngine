//
// Created by Nikolay Kanchevski on 10.10.22.
//

#pragma once

#include "Light.h"
#include "../../Classes/IdentifierPool.h"
#include "../../../Core/Rendering/UI/ImGuiUtilities.h"
#include "../../../Core/Rendering/Math/MatrixUtilities.h"

namespace Sierra::Engine::Components
{

    /// @brief A component class representing a directional light in the scene. Derives from both <see cref="Light"/> and <see cref="Component"/>.
    class DirectionalLight : public Light
    {
    public:
        /* --- CONSTRUCTORS --- */
        inline DirectionalLight()
        {
            lightID = IDPool.CreateNewID();
        };

        inline void OnAddComponent() override
        {
            GetComponent<Transform>().PushOnDirtyCallback([this]{
                Recalculate();
            });
        }

        /* --- POLLING METHODS --- */
        inline void OnDrawUI() override
        {
            GUI::BeginProperties();

            GUI::FloatProperty("Intensity:", intensity);

            static const float resetValues[3] = { 0.0f, 0.0f, 0.0f };
            static const char* tooltips[3] = { "Some tooltip.", "Some tooltip.", "Some tooltip." };
            GUI::PropertyVector3("Color:", color, resetValues, tooltips);

            GUI::EndProperties();
        }

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline static uint GetDirectionalLightCount() { return IDPool.GetTotalIDsCount(); }

        /* --- DESTRUCTOR --- */
        inline void Destroy() override { IDPool.RemoveID(lightID); };

        struct ShaderDirectionalLight
        {
            Matrix4x4 projectionView;

            Vector3 direction;
            float intensity;

            Vector3 color;
            float _algin1_;
        };

        operator ShaderDirectionalLight() const noexcept { return
        {
            .projectionView = projectionView,
            .direction = direction,
            .intensity = intensity,
            .color = color
        }; }

    private:
        Vector3 direction = { 0.0f, 1.0f, 0.0f };

        inline void Recalculate() override
        {
            Transform &transform = GetComponent<Transform>();

            Vector3 rotation = transform.GetWorldRotation();
            float cosYaw = glm::cos(glm::radians(rotation.x));
            float sinYaw = glm::sin(glm::radians(rotation.x));
            float cosPitch = glm::cos(glm::radians(rotation.y));
            float sinPitch = glm::sin(glm::radians(rotation.y));

            direction.x = cosYaw * cosPitch;
            direction.y = sinPitch;
            direction.z = sinYaw * cosPitch;
            direction = glm::normalize(direction);

            using namespace Core::Rendering;
            this->projectionView = MatrixUtilities::CreateOrthographicProjectionMatrix(30.0f, -220.0f, 80.0f) * MatrixUtilities::CreateViewMatrix(transform.GetWorldPosition(), direction);
        }

        inline static auto IDPool = Classes::IdentifierPool<uint>(MAX_DIRECTIONAL_LIGHTS);
    };

}