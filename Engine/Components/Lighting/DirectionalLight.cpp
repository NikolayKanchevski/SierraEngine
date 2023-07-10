//
// Created by Nikolay Kanchevski on 29.06.23.
//

#include "DirectionalLight.h"

#include "../Transform.h"
#include "../../Classes/Math.h"
#include "../../../Core/Rendering/UI/ImGuiUtilities.h"

namespace Sierra::Engine
{

    /* --- CONSTRUCTORS --- */

    DirectionalLight::DirectionalLight()
    {
        lightID = IDPool.CreateNewID();
    }

    /* --- POLLING METHODS --- */

    void DirectionalLight::OnDrawUI()
    {
        using namespace Rendering;
        GUI::BeginProperties();

        GUI::FloatProperty("Intensity:", intensity);

        static const float resetValues[3] = { 0.0f, 0.0f, 0.0f };
        static const char* tooltips[3] = { "Some tooltip.", "Some tooltip.", "Some tooltip." };
        GUI::PropertyVector3("Color:", color, resetValues, tooltips);

        GUI::EndProperties();
    }

    void DirectionalLight::Recalculate()
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

        projectionView = Math::CreateOrthographicProjectionMatrix(30.0f, -220.0f, 80.0f) * Math::CreateViewMatrix(transform.GetWorldPosition(), direction);
    }

    /* --- DESTRUCTORS --- */

    void DirectionalLight::Destroy()
    {
        IDPool.RemoveID(lightID);
    }

    /* --- OPERATORS --- */

    DirectionalLight::operator ShaderDirectionalLight() const
    {
        return
        {
            .projectionView = projectionView,
            .direction = direction,
            .intensity = intensity,
            .color = color
        };
    }
}