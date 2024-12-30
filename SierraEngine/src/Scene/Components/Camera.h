//
// Created by Nikolay Kanchevski on 25.10.24.
//

#pragma once

#include "../Component.h"

#include "Transform.h"

namespace SierraEngine
{

    enum class ProjectionType : bool
    {
        Perspective,
        Orthographic
    };

    class Camera final : public Component
    {
    public:
        /* --- CONSTRUCTORS --- */
        Camera() noexcept = default;

        /* --- SETTER METHODS --- */
        void SetProjectionType(ProjectionType projectionType);
        void SetNearClip(float32 nearClip);
        void SetFarClip(float32 farClip);
        void SetFieldOfView(float32 fieldOfView);

        /* --- GETTER METHODS --- */
        [[nodiscard]] Matrix4x4 GetViewMatrix(const Transform& transform) const noexcept;
        [[nodiscard]] Matrix4x4 GetProjectionMatrix(float32 width, float32 height) const noexcept;

        [[nodiscard]] ProjectionType GetProjectionType() const noexcept { return projectionType; }
        [[nodiscard]] float32 GetNearClip() const noexcept { return nearClip; }
        [[nodiscard]] float32 GetFarClip() const noexcept { return farClip; }
        [[nodiscard]] float32 GetFieldOfView() const noexcept { return fieldOfView; }

        /* --- TYPE DATA --- */
        [[nodiscard]] constexpr static std::string_view GetName() { return "Camera"; }

        /* --- COPY SEMANTICS --- */
        Camera(const Camera&) = delete;
        Camera& operator=(const Camera&) = delete;

        /* --- MOVE SEMANTICS --- */
        Camera(Camera&&) noexcept = default;
        Camera& operator=(Camera&&) noexcept = default;

        /* --- DESTRUCTORS --- */
        ~Camera() noexcept = default;

    private:
        ProjectionType projectionType = ProjectionType::Perspective;
        float32 nearClip = 0.01f;
        float32 farClip = 200.0f;
        float32 fieldOfView = 45.0f;

    };

}