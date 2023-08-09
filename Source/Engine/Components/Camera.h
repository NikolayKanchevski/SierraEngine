//
// Created by Nikolay Kanchevski on 11.10.22.
//

#pragma once

#include "Component.h"

namespace Sierra::Engine
{

    class Camera : public Component
    {
    public:
        /* --- CONSTRUCTORS --- */
        Camera() = default;
        void OnAddComponent() override;

        /* --- POLLING METHODS --- */
        void OnDrawUI() override;

        /* --- SETTER METHODS --- */
        void CalculateViewMatrix();
        void CalculateProjectionMatrix();

        void SetFOV(float givenFOV);
        void SetNearClip(float givenClip);
        void SetFarClip(float givenClip);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline float GetFOV() const { return FOV; }
        [[nodiscard]] inline float GetNearClip() const { return nearClip; }
        [[nodiscard]] inline float GetFarClip() const { return farClip; }

        [[nodiscard]] float GetYaw() const;
        [[nodiscard]] float GetPitch() const;
        [[nodiscard]] float GetRoll() const;
        [[nodiscard]] Vector3 GetYawPitchRoll() const;

        [[nodiscard]] inline Matrix4x4 GetViewMatrix() { return viewMatrix; };
        [[nodiscard]] inline Matrix4x4 GetProjectionMatrix() { if (isProjectionDirty) CalculateProjectionMatrix(); return projectionMatrix; };

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

    private:
        float FOV = 45.0f;
        float nearClip = 0.01f;
        float farClip = 200.0f;

        Matrix4x4 viewMatrix = Matrix4x4(1.0f);
        Matrix4x4 projectionMatrix = Matrix4x4(1.0f);
        Matrix4x4 inverseViewMatrix = Matrix4x4(1.0f);
        Matrix4x4 inverseProjectionMatrix = Matrix4x4(1.0f);

        bool isProjectionDirty = false;

    public:
        static inline const Vector3 upDirection = { 0.0f, 1.0f, 0.0f };

    };
}
