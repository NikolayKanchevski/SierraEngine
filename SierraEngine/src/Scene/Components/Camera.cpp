//
// Created by Nikolay Kanchevski on 25.10.24.
//

#include "Camera.h"

namespace SierraEngine
{

    /* --- SETTER METHODS --- */

    void Camera::SetProjectionType(const ProjectionType givenProjectionType)
    {
        projectionType = givenProjectionType;
    }

    void Camera::SetNearClip(const float32 givenNearClip)
    {
        nearClip = givenNearClip;
    }

    void Camera::SetFarClip(const float32 givenFarClip)
    {
        farClip = givenFarClip;
    }

    void Camera::SetFieldOfView(const float32 givenFieldOfView)
    {
        fieldOfView = givenFieldOfView;
    }

    /* --- SETTER METHODS --- */

    Matrix4x4 Camera::GetViewMatrix(const Transform& transform) const noexcept
    {
        return glm::lookAt(transform.GetPosition(), transform.GetPosition() + transform.GetForwardDirection(), Transform::UP_DIRECTION);
    }

    Matrix4x4 Camera::GetProjectionMatrix(const float32 width, const float32 height) const noexcept
    {
        const float32 aspect = width / height;
        switch (projectionType)
        {
            case ProjectionType::Perspective:
            {
                return glm::perspective(glm::radians(fieldOfView), aspect, nearClip, farClip);
            }
            case ProjectionType::Orthographic:
            {
                const float32 orthographicSize = height / 64.0f;
                const float32 orthographicWidth = orthographicSize * aspect;
                const float32 orthographicHeight = orthographicSize;

                return glm::ortho(-orthographicWidth * 0.5f, orthographicWidth * 0.5f, -orthographicHeight * 0.5f, orthographicHeight * 0.5f, nearClip, farClip);
            }
        }
    }

}