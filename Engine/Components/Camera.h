//
// Created by Nikolay Kanchevski on 11.10.22.
//

#pragma once

#include <glm/glm.hpp>

#include "Component.h"

namespace Sierra::Engine::Components
{

    class Camera : public Component
    {
    public:
        /* --- PROPERTIES --- */
        float fov = 45.0f;

        float nearClip = 0.1f;
        float farClip = 200.0f;

        /* --- CONSTRUCTORS --- */
        Camera();

        /* --- SETTER METHODS --- */
        void SetAsMain();

        /* --- GETTER METHODS --- */
        [[nodiscard]] glm::vec3 GetFrontDirection() const;

        [[nodiscard]] glm::mat4x4 GetViewMatrix() const;
        [[nodiscard]] glm::mat4x4 GetProjectionMatrix() const;

        [[nodiscard]] inline glm::vec3 GetBackDirection() const
        { return -GetFrontDirection(); }

        [[nodiscard]] inline glm::vec3 GetLeftDirection() const
        { return glm::cross(-GetFrontDirection(), upDirection); }

        [[nodiscard]] inline glm::vec3 GetRightDirection() const
        { return glm::cross(GetFrontDirection(), upDirection); }

        [[nodiscard]] inline glm::vec3 GetUpDirection() const
        { return upDirection; }

        [[nodiscard]] inline glm::vec3 GetDownDirection() const
        { return -upDirection; }

        [[nodiscard]] static inline Camera* GetMainCamera()
        { return mainCamera; }

        /* --- DESTRUCTOR --- */
        inline void Destroy() const override { RemoveComponent<Camera>(); }

    private:
        static Camera *mainCamera;

        static inline const glm::vec3 upDirection = { 0.0f, 1.0f, 0.0f };
    };
}
