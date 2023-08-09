//
// Created by Nikolay Kanchevski on 29.06.23.
//

#include "DirectionalLight.h"

#include "Transform.h"
#include "../Classes/Math.h"
#include "../../Editor/GUI.h"

namespace Sierra::Engine
{

    /* --- POLLING METHODS --- */

    void DirectionalLight::OnDrawUI()
    {
        using namespace Editor;
        GUI::BeginProperties();

        GUI::FloatProperty("Intensity:", intensity);

        GUI::Vector3Property("Color:", color, "Some Tooltip");

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
        direction.y = -sinPitch;
        direction.z = sinYaw * cosPitch;
        direction = glm::normalize(direction);

        projectionView = Math::CreateOrthographicProjectionMatrix(30.0f, -220.0f, 80.0f) * Math::CreateViewMatrix(transform.GetWorldPosition(), direction);
    }

}