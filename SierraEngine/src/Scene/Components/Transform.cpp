//
// Created by Nikolay Kanchevski on 27.10.24.
//

#include "Transform.h"

namespace SierraEngine
{

    /* --- SETTER METHODS --- */

    void Transform::SetPosition(const Vector3 givenPosition) noexcept
    {
        position = givenPosition;
    }

    void Transform::SetRotation(const Vector3 givenRotation) noexcept
    {
        rotation = givenRotation;
    }

    void Transform::SetScale(const Vector3 givenScale) noexcept
    {
        scale = givenScale;
    }

    /* --- GETTER METHODS --- */

    Matrix4x4 Transform::GetTransformationMatrix() const noexcept
    {
        return glm::translate(Matrix4x4(1.0f), position) * glm::toMat4(GetOrientation()) * glm::scale(Matrix4x4(1.0f), scale);
    }

    Quaternion Transform::GetOrientation() const noexcept
    {
        return glm::inverse(glm::quat({ glm::radians(rotation.y), glm::radians(rotation.x), glm::radians(rotation.z) }));
    }

}