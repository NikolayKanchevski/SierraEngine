//
// Created by Nikolay Kanchevski on 27.10.24.
//

#pragma once

#include "../Component.h"

namespace SierraEngine
{

    class Transform final : public Component
    {
    public:
        /* --- CONSTRUCTORS --- */
        Transform() noexcept = default;

        /* --- SETTER METHODS --- */
        void SetPosition(Vector3 position) noexcept;
        void SetRotation(Vector3 rotation) noexcept;
        void SetScale(Vector3 scale) noexcept;

        void SetPositionX(const float32 x) noexcept { SetPosition({ x, GetPositionY(), GetPositionZ() }); }
        void SetPositionY(const float32 y) noexcept { SetPosition({ GetPositionX(), y, GetPositionZ() }); }
        void SetPositionZ(const float32 z) noexcept { SetPosition({ GetPositionX(), GetPositionY(), z }); }

        void SetRotationX(const float32 x) noexcept { SetRotation({ x, GetRotationY(), GetRotationZ() }); }
        void SetRotationY(const float32 y) noexcept { SetRotation({ GetRotationX(), y, GetRotationZ() }); }
        void SetRotationZ(const float32 z) noexcept { SetRotation({ GetRotationX(), GetRotationY(), z }); }

        void SetScaleX(const float32 x) noexcept { SetScale({ x, GetScaleY(), GetScaleZ() }); }
        void SetScaleY(const float32 y) noexcept { SetScale({ GetScaleX(), y, GetScaleZ() }); }
        void SetScaleZ(const float32 z) noexcept { SetScale({ GetScaleX(), GetScaleY(), z }); }

        /* --- GETTER METHODS --- */
        [[nodiscard]] Matrix4x4 GetTransformationMatrix() const noexcept;
        [[nodiscard]] Quaternion32 GetOrientation() const noexcept;

        [[nodiscard]] Vector3 GetPosition() const noexcept { return position; }
        [[nodiscard]] Vector3 GetRotation() const noexcept { return rotation; }
        [[nodiscard]] Vector3 GetScale() const noexcept { return scale; }

        [[nodiscard]] float32 GetPositionX() const noexcept { return GetPosition().x; }
        [[nodiscard]] float32 GetPositionY() const noexcept { return GetPosition().y; }
        [[nodiscard]] float32 GetPositionZ() const noexcept { return GetPosition().z; }

        [[nodiscard]] float32 GetRotationX() const noexcept { return GetRotation().x; }
        [[nodiscard]] float32 GetRotationY() const noexcept { return GetRotation().y; }
        [[nodiscard]] float32 GetRotationZ() const noexcept { return GetRotation().z; }

        [[nodiscard]] float32 GetScaleX() const noexcept { return GetScale().x; }
        [[nodiscard]] float32 GetScaleY() const noexcept { return GetScale().y; }
        [[nodiscard]] float32 GetScaleZ() const noexcept { return GetScale().z; }

        [[nodiscard]] Vector3 GetForwardDirection() const noexcept { return GetOrientation() * FORWARD_DIRECTION; }
        [[nodiscard]] Vector3 GetBackwardDirection() const noexcept { return -GetForwardDirection(); }

        [[nodiscard]] Vector3 GetLeftDirection() const noexcept { return GetOrientation() * RIGHT_DIRECTION; }
        [[nodiscard]] Vector3 GetRightDirection() const noexcept { return -GetLeftDirection(); }

        [[nodiscard]] Vector3 GetUpDirection() const noexcept { return GetOrientation() * UP_DIRECTION; }
        [[nodiscard]] Vector3 GetDownDirection() const noexcept { return -GetUpDirection(); }

        /* --- CONSTANTS --- */
        constexpr static Vector3 FORWARD_DIRECTION = { 0.0f, 0.0f, 1.0f };
        constexpr static Vector3 BACKWARD_DIRECTION = -FORWARD_DIRECTION;

        constexpr static Vector3 RIGHT_DIRECTION = { 1.0f, 0.0f, 0.0f };
        constexpr static Vector3 LEFT_DIRECTION = -RIGHT_DIRECTION;

        constexpr static Vector3 UP_DIRECTION = { 0.0f, 1.0f, 0.0f };
        constexpr static Vector3 DOWN_DIRECTION = -UP_DIRECTION;

        /* --- TYPE DATA --- */
        [[nodiscard]] constexpr static std::string_view GetName() { return "Transform"; }

        /* --- COPY SEMANTICS --- */
        Transform(const Transform&) = delete;
        Transform& operator=(const Transform&) = delete;

        /* --- MOVE SEMANTICS --- */
        Transform(Transform&&) noexcept = default;
        Transform& operator=(Transform&&) noexcept = default;

        /* --- DESTRUCTORS --- */
        ~Transform() noexcept = default;

    private:
        Vector3 position = { 0.0f, 0.0f, 0.0f };
        Vector3 rotation = { 0.0f, 0.0f, 0.0f };
        Vector3 scale = { 1.0f, 1.0f, 1.0f };

    };

}