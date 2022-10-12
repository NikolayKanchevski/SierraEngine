//
// Created by Nikolay Kanchevski on 11.10.22.
//

#include "Camera.h"

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

}
