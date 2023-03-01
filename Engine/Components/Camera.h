//
// Created by Nikolay Kanchevski on 11.10.22.
//

#pragma once

#include "Component.h"

namespace Sierra::Engine::Components
{

    class Camera : public Component
    {
    public:
        /* --- PROPERTIES --- */
        float fov = 45.0f;

        float nearClip = 0.01f;
        float farClip = 200.0f;

        /* --- CONSTRUCTORS --- */
        Camera() = default;
        void OnAddComponent() override;

        /* --- POLLING METHODS --- */
        void OnDrawUI() override;

        /* --- SETTER METHODS --- */
        void SetAsMain();
        void CalculateViewMatrices();
        void CalculateProjectionMatrices();

        /* --- GETTER METHODS --- */
        [[nodiscard]] float GetYaw() const;
        [[nodiscard]] float GetPitch() const;
        [[nodiscard]] float GetRoll() const;
        [[nodiscard]] Vector3 GetYawPitchRoll() const;

        [[nodiscard]] inline Matrix4x4 GetViewMatrix() { return viewMatrix; };
        [[nodiscard]] inline Matrix4x4 GetProjectionMatrix()
        {
            // TODO: Add a callback to only recalculate this on resize
            CalculateProjectionMatrices();
            return projectionMatrix;
        };

        [[nodiscard]] inline Matrix4x4 GetInverseViewMatrix() { return inverseViewMatrix; };
        [[nodiscard]] inline Matrix4x4 GetInverseProjectionMatrix() { return inverseProjectionMatrix; }

        [[nodiscard]] Vector3 GetFrontDirection() const;

        [[nodiscard]] inline Vector3 GetBackDirection() const
        { return -GetFrontDirection(); }

        [[nodiscard]] inline Vector3 GetLeftDirection() const
        { return glm::cross(-GetFrontDirection(), upDirection); }

        [[nodiscard]] inline Vector3 GetRightDirection() const
        { return glm::cross(GetFrontDirection(), upDirection); }

        [[nodiscard]] inline Vector3 GetUpDirection() const
        { return upDirection; }

        [[nodiscard]] inline Vector3 GetDownDirection() const
        { return -upDirection; }

        [[nodiscard]] static inline Camera& GetMainCamera()
        { return World::GetComponent<Camera>(mainCamera); }

    private:
        Matrix4x4 viewMatrix;
        Matrix4x4 projectionMatrix;
        Matrix4x4 inverseViewMatrix;
        Matrix4x4 inverseProjectionMatrix;

        static inline entt::entity mainCamera = entt::null;
        static inline const Vector3 upDirection = { 0.0f, 1.0f, 0.0f };
    };
}
