//
// Created by Nikolay Kanchevski on 11.10.22.
//

#include "Camera.h"

#include "Transform.h"
#include "../../Core/Rendering/UI/ImGuiCore.h"
#include "../../Core/Rendering/UI/ImGuiUtilities.h"
#include "../../Core/Rendering/Math/MatrixUtilities.h"

using Sierra::Core::Rendering::UI::ImGuiCore;

namespace Sierra::Engine::Components
{
    /* --- CONSTRUCTORS --- */

    void Camera::OnAddComponent()
    {
        CalculateViewMatrix();
        CalculateProjectionMatrix();

        GetComponent<Transform>().PushOnDirtyCallback([this]
        {
            CalculateViewMatrix();
        });

        if (mainCamera == entt::null) SetAsMain();
    }

    /* --- POLLING METHODS --- */

    void Camera::OnDrawUI()
    {
        GUI::BeginProperties();

        if (GUI::FloatProperty("FOV:", FOV, "Some Tooltip")) isProjectionDirty = true;
        if (GUI::FloatProperty("Near Clip:", nearClip, "Some Tooltip")) isProjectionDirty = true;
        if (GUI::FloatProperty("Far Clip:", farClip, "Some Tooltip")) isProjectionDirty = true;

        GUI::EndProperties();
    }

    /* --- SETTER METHODS --- */

    void Camera::SetAsMain()
    {
        mainCamera = enttEntity;
    }

    /* --- GETTER METHODS --- */

    Vector3 Camera::GetFrontDirection() const
    {
        Transform &transform = GetComponent<Transform>();

        Vector3 rotation = transform.GetWorldRotation();
        float cosYaw = glm::cos(glm::radians(rotation.x));
        float sinYaw = glm::sin(glm::radians(rotation.x));
        float cosPitch = glm::cos(glm::radians(rotation.y));
        float sinPitch = glm::sin(glm::radians(rotation.y));

        Vector3 direction;
        direction.x = cosYaw * cosPitch;
        direction.y = sinPitch;
        direction.z = sinYaw * cosPitch;

        return glm::normalize(direction);
    }

    float Camera::GetYaw() const
    {
        return GetComponent<Transform>().GetWorldRotation().x;
    }

    float Camera::GetPitch() const
    {
        return GetComponent<Transform>().GetWorldRotation().y;
    }

    float Camera::GetRoll() const
    {
        return GetComponent<Transform>().GetWorldRotation().z;
    }

    Vector3 Camera::GetYawPitchRoll() const
    {
        return GetComponent<Transform>().GetWorldRotation();
    }

    void Camera::CalculateViewMatrix()
    {
        Transform &transform = GetComponent<Transform>();
        Vector3 frontDirection = GetFrontDirection();

        Vector3 rendererCameraPosition = transform.GetWorldPositionUpInverted();
        Vector3 rendererCameraFrontDirection = { frontDirection.x, -frontDirection.y, frontDirection.z };

        using namespace Core::Rendering;
        viewMatrix = MatrixUtilities::CreateViewMatrix(rendererCameraPosition, rendererCameraFrontDirection);
        inverseViewMatrix = glm::inverse(viewMatrix);
    }

    void Camera::CalculateProjectionMatrix()
    {
        using namespace Core::Rendering;
        projectionMatrix = MatrixUtilities::CreateProjectionMatrix(FOV, static_cast<float>(ImGuiCore::GetSceneViewWidth()) / static_cast<float>(ImGuiCore::GetSceneViewHeight()), nearClip, farClip);
        inverseProjectionMatrix = glm::inverse(projectionMatrix);
        isProjectionDirty = false;
    }

    #define CHECK_CHANGE(a, b) if (a == b) return; else { a = b; isProjectionDirty = true; }

    void Camera::SetFOV(const float givenFOV)
    {
        CHECK_CHANGE(FOV, givenFOV)
    }

    void Camera::SetNearClip(const float givenClip)
    {
        CHECK_CHANGE(nearClip, givenClip)
    }

    void Camera::SetFarClip(const float givenClip)
    {
        CHECK_CHANGE(farClip, givenClip)
    }

}
