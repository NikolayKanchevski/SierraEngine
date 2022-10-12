//
// Created by Nikolay Kanchevski on 11.10.22.
//

#pragma once

#include <glm/glm.hpp>
#include "Transform.h"

namespace Sierra::Engine::Components
{

    class Camera
    {
    public:
        /* --- PROPERTIES --- */
        Transform transform{};

        float fov = 45.0f;

        float nearClip = 0.1f;
        float farClip = 100.0f;

        /* --- CONSTRUCTORS --- */
        Camera();

        /* --- SETTER METHODS --- */
        void SetAsMain();

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline glm::vec3 GetFrontDirection() const
        { return this->frontDirection; }

        [[nodiscard]] inline glm::vec3 GetUpDirection() const
        { return this->upDirection; }

        [[nodiscard]] inline glm::vec3 GetLeftDirection() const
        { return glm::cross(frontDirection, upDirection); }

        [[nodiscard]] inline glm::vec3 GetRightDirection() const
        { return glm::cross(-frontDirection, upDirection); }

        [[nodiscard]] static inline Camera* GetMainCamera()
        { return mainCamera; }

        glm::vec3 frontDirection = {0.0f, 0.0f, -1.0f };
    private:
        static Camera *mainCamera;

        glm::vec3 upDirection = {0.0f, 1.0f, 0.0f };
    };
}
