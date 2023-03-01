//
// Created by Nikolay Kanchevski on 18.01.23.
//

#include "Transform.h"

#include "Relationship.h"
#include "../../Core/Rendering/UI/ImGuiUtilities.h"
#include "../../Core/Rendering/Math/MatrixUtilities.h"
#include "MeshRenderer.h"

#define CHECK_CHANGE(a, b) if (a == b) return; else { a = b; isDirty = true; }

namespace Sierra::Engine::Components
{

    /* --- SETTER METHODS --- */

    void Transform::SetPosition(const Vector3 newPosition) { CHECK_CHANGE(localPosition, newPosition) }
    void Transform::SetRotation(const Vector3 newRotation) { CHECK_CHANGE(localRotation, newRotation) }
    void Transform::SetScale(const Vector3 newScale) { CHECK_CHANGE(localScale, newScale) }

    void Transform::SetPosition(std::optional<float> xPosition, std::optional<float> yPosition, std::optional<float> zPosition)
    {
        SetPosition({ xPosition ? xPosition.value() :  localPosition.x, yPosition ? yPosition.value() : localPosition.y, zPosition ? zPosition.value() : localPosition.z });
    }

    void Transform::SetRotation(const std::optional<float> xRotation, const std::optional<float> yRotation, const std::optional<float> zRotation)
    {
        SetRotation({ xRotation ? xRotation.value() :  localRotation.x, yRotation ? yRotation.value() : localRotation.y, zRotation ? zRotation.value() : localRotation.z });
    }

    void Transform::SetScale(const std::optional<float> xScale, const std::optional<float> yScale, const std::optional<float> zScale)
    {
        SetScale({ xScale ? xScale.value() : localScale.x, yScale ? yScale.value() : localScale.y, zScale ? zScale.value() : localScale.z });
    }

    void Transform::SetWorldPosition(const Vector3 newPosition)
    {
        if (newPosition == position) return;

        isDirty = true;

        if (HasParentTransform())
        {
            localPosition = newPosition - GetParentTransform().position;
        }
        else
        {
            position = newPosition;
            localPosition = newPosition;
        }
    }

    void Transform::SetWorldPosition(const std::optional<float> xPosition, const std::optional<float> yPosition, const std::optional<float> zPosition)
    {
        SetWorldPosition({ xPosition ? xPosition.value() : position.x, yPosition ? yPosition.value() : position.y, zPosition ? zPosition.value() : position.z });
    }

    void Transform::SetWorldRotation(const Vector3 newRotation)
    {
        isDirty = newRotation != rotation;

        if (HasParentTransform())
        {
            localRotation = newRotation - GetParentTransform().rotation;
        }
        else
        {
            rotation = newRotation;
            localRotation = newRotation;
        }
    }

    void Transform::SetWorldRotation(const std::optional<float> xRotation, const std::optional<float> yRotation, const std::optional<float> zRotation)
    {
        SetWorldRotation({xRotation ? xRotation.value() : rotation.x, yRotation ? yRotation.value() : rotation.y, zRotation ? zRotation.value() : rotation.z });
    }

    void Transform::SetWorldScale(const Vector3 newScale)
    {
        isDirty = newScale != scale;

        if (HasParentTransform())
        {
            localScale = newScale / GetParentTransform().scale;
        }
        else
        {
            scale = newScale;
            localScale = newScale;
        }
    }

    void Transform::SetWorldScale(const std::optional<float> xScale, const std::optional<float> yScale, const std::optional<float> zScale)
    {
        SetWorldScale({xScale ? xScale.value() : scale.x, yScale ? yScale.value() : scale.y, zScale ? zScale.value() : scale.z });
    }

    void Transform::LookAt(const Vector3 point)
    {

    }

    void Transform::RotateAround(const Vector3 point, const Vector3 axis, float angle)
    {

    }

    /* --- GETTER  METHODS --- */

    Matrix4x4& Transform::GetModelMatrix()
    {
        ASSERT_ERROR_IF(!HasComponent<MeshRenderer>(), "Cannot get a model matrix for an entity that does not contain the MeshRenderer component");
        return modelMatrix.value();
    }

    bool Transform::HasParentTransform() const
    {
        entt::entity parentEntity = GetComponent<Relationship>().GetEnttParentEntity();
        return parentEntity != entt::null && World::HasComponent<Transform>(parentEntity);
    }

    Transform& Transform::GetParentTransform() const
    {
        return World::GetComponent<Transform>(GetComponent<Relationship>().GetEnttParentEntity());
    }

    Transform& Transform::GetChildTransform(const uint childIndex) const
    {
        return World::GetComponent<Transform>(GetComponent<Relationship>().GetEnttChildrenEntities()[childIndex]);
    }

    Transform& Transform::GetOriginTransform() const
    {
        return World::GetComponent<Transform>(GetComponent<Relationship>().GetEnttOriginParentEntity());
    }

    /* --- POLLING METHODS --- */

    void Transform::UpdateChain()
    {
        if (isDirty)
        {
            RecalculateOrigin();
            for (const auto &child : World::GetComponent<Relationship>(enttEntity).GetEnttChildrenEntities())
            {
                RecalculateChainDeeper(child, enttEntity);
            }

            isDirty = false;
        }
        else
        {
            UpdateChainDeeper(enttEntity);
        }
    }

    void Transform::UpdateChainDeeper(const entt::entity currentChild)
    {
        for (const auto &subChild : World::GetComponent<Relationship>(currentChild).GetEnttChildrenEntities())
        {
            if (!World::GetComponent<Transform>(subChild).isDirty) UpdateChainDeeper(subChild);
            else RecalculateChainDeeper(subChild, currentChild);
        }
    }

    void Transform::RecalculateChainDeeper(const entt::entity currentChild, const entt::entity parent)
    {
        Transform &parentTransform = World::GetComponent<Transform>(parent);
        World::GetComponent<Transform>(currentChild).Recalculate(parentTransform);

        for (const auto &subChild : World::GetComponent<Relationship>(currentChild).GetEnttChildrenEntities())
        {
            RecalculateChainDeeper(subChild, currentChild);
        }
    }

    void Transform::DoCallbacks()
    {
        for (const auto &Callback : OnChangeCallbacks)
        {
            Callback();
        }
    }

    void Transform::Recalculate(const Transform &parentTransform)
    {
        position = localPosition + parentTransform.position;
        rotation = localRotation + parentTransform.rotation;
        scale = localScale * parentTransform.scale;

        quaternion = Quaternion({glm::radians(-rotation.y), glm::radians(rotation.x), glm::radians(rotation.z) });
        forwardDirection.x = 2 * (quaternion.x * quaternion.z + quaternion.w * quaternion.y);
        forwardDirection.y = 2 * (quaternion.y * quaternion.z - quaternion.w * quaternion.x);
        forwardDirection.z = 1 - 2 * (quaternion.x * quaternion.x + quaternion.y * quaternion.y);

        upDirection.x = 2 * (quaternion.x * quaternion.y - quaternion.w * quaternion.z);
        upDirection.y = 1 - 2 * (quaternion.x * quaternion.x + quaternion.z * quaternion.z);
        upDirection.z = 2 * (quaternion.y * quaternion.z + quaternion.w * quaternion.x);

        if (HasComponent<MeshRenderer>()) modelMatrix = MatrixUtilities::CreateModel(GetWorldPositionUpInverted(), rotation, scale);

        DoCallbacks();

        isDirty = false;
    }

    void Transform::RecalculateOrigin()
    {
        position = localPosition;
        rotation = localRotation;
        scale = localScale;

        quaternion = Quaternion({glm::radians(-rotation.y), glm::radians(rotation.x), glm::radians(rotation.z) });
        forwardDirection.x = 2 * (quaternion.x * quaternion.z + quaternion.w * quaternion.y);
        forwardDirection.y = 2 * (quaternion.y * quaternion.z - quaternion.w * quaternion.x);
        forwardDirection.z = 1 - 2 * (quaternion.x * quaternion.x + quaternion.y * quaternion.y);

        upDirection.x = 2 * (quaternion.x * quaternion.y - quaternion.w * quaternion.z);
        upDirection.y = 1 - 2 * (quaternion.x * quaternion.x + quaternion.z * quaternion.z);
        upDirection.z = 2 * (quaternion.y * quaternion.z + quaternion.w * quaternion.x);

        if (HasComponent<MeshRenderer>()) modelMatrix = MatrixUtilities::CreateModel(GetWorldPositionUpInverted(), rotation, scale);

        DoCallbacks();
    }

    void Transform::OnDrawUI()
    {
        GUI::BeginProperties(ImGuiTableFlags_BordersInnerV);

        static float resetValues[3];
        static const char* tooltips[3];

        resetValues[0] = 0.0f;
        resetValues[1] = 0.0f;
        resetValues[2] = 0.0f;

        tooltips[0] = "Some tooltip.";
        tooltips[1] = "Some tooltip.";
        tooltips[2] = "Some tooltip.";
        if (GUI::PropertyVector3("Position:", localPosition, resetValues, tooltips)) isDirty = true;

        resetValues[0] = 0.0f;
        resetValues[1] = 0.0f;
        resetValues[2] = 0.0f;

        tooltips[0] = "Some tooltip.";
        tooltips[1] = "Some tooltip.";
        tooltips[2] = "Some tooltip.";
        if (GUI::PropertyVector3("Rotation:", localRotation, resetValues, tooltips)) isDirty = true;

        resetValues[0] = 1.0f;
        resetValues[1] = 1.0f;
        resetValues[2] = 1.0f;

        tooltips[0] = "Some tooltip.";
        tooltips[1] = "Some tooltip.";
        tooltips[2] = "Some tooltip.";
        if (GUI::PropertyVector3("Scale:", localScale, resetValues, tooltips)) isDirty = true;

        GUI::EndProperties();
    }

    void Transform::PushOnChangeCallback(Callback callback)
    {
        OnChangeCallbacks.push_back(callback);
    }

    void Transform::PopOnChangeCallback()
    {
        OnChangeCallbacks.pop_back();
    }


}