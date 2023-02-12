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
        void CalculateViewMatrix();
        void CalculateProjectionMatrix();

        /* --- GETTER METHODS --- */
        [[nodiscard]] float GetYaw() const;
        [[nodiscard]] float GetPitch() const;
        [[nodiscard]] float GetRoll() const;
        [[nodiscard]] Vector3 GetYawPitchRoll() const;

        [[nodiscard]] Matrix4x4 GetViewMatrix();
        [[nodiscard]] Matrix4x4 GetProjectionMatrix();

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

        [[nodiscard]] static inline Camera* GetMainCamera()
        { return mainCamera; }

        /* --- DESTRUCTOR --- */
        inline void Destroy() const override { RemoveComponent<Camera>(); }

    private:
        Matrix4x4 viewMatrix;
        Matrix4x4 projectionMatrix;

        static Camera *mainCamera;
        static inline const Vector3 upDirection = { 0.0f, 1.0f, 0.0f };
    };
}
