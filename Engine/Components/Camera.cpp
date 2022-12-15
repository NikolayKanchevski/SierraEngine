//
// Created by Nikolay Kanchevski on 11.10.22.
//

#include "Camera.h"

#include <glm/vec3.hpp>

namespace Sierra::Engine::Components
{
    Camera *Camera::mainCamera = nullptr;

    /* --- CONSTRUCTORS --- */

    Camera::Camera()
    {
        if (mainCamera != nullptr) return;

        SetAsMain();
    }

    /* --- SETTER METHODS --- */

    void Camera::SetAsMain()
    {
        mainCamera = this;
    }

    void Camera::SetFrontDirection(const glm::vec3 givenFrontDirection)
    {
        this->frontDirection = givenFrontDirection;
    }

}
