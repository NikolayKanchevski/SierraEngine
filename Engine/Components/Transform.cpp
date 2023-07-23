//
// Created by Nikolay Kanchevski on 18.01.23.
//

#include "Transform.h"

#include "Relationship.h"
#include "../../Editor/GUI.h"

#define CHECK_CHANGE(OLD_VALUE, NEW_VALUE, CHANGE) if (OLD_VALUE == NEW_VALUE) { return; } else { OLD_VALUE = NEW_VALUE; dirtyFlag |= CHANGE; }

namespace Sierra::Engine
{
    /* --- SETTER METHODS --- */

    void Transform::SetPosition(const Vector3 newPosition) { CHECK_CHANGE(localPosition, newPosition, TransformDirtyFlag::POSITION) }
    void Transform::SetRotation(const Vector3 newRotation) { CHECK_CHANGE(localRotation, newRotation, TransformDirtyFlag::ROTATION) }
    void Transform::SetScale(const Vector3 newScale) { CHECK_CHANGE(localScale, newScale, TransformDirtyFlag::SCALE) }

    void Transform::SetPosition(Optional<float> xPosition, Optional<float> yPosition, Optional<float> zPosition)
    {
        SetPosition({ xPosition ? xPosition.value() :  localPosition.x, yPosition ? yPosition.value() : localPosition.y, zPosition ? zPosition.value() : localPosition.z });
    }

    void Transform::SetRotation(const Optional<float> xRotation, const Optional<float> yRotation, const Optional<float> zRotation)
    {
        SetRotation({ xRotation ? xRotation.value() :  localRotation.x, yRotation ? yRotation.value() : localRotation.y, zRotation ? zRotation.value() : localRotation.z });
    }

    void Transform::SetScale(const Optional<float> xScale, const Optional<float> yScale, const Optional<float> zScale)
    {
        SetScale({ xScale ? xScale.value() : localScale.x, yScale ? yScale.value() : localScale.y, zScale ? zScale.value() : localScale.z });
    }

    void Transform::SetWorldPosition(const Vector3 newPosition)
    {
        if (newPosition == position) return;

        dirtyFlag |= TransformDirtyFlag::POSITION;
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

    void Transform::SetWorldPosition(const Optional<float> xPosition, const Optional<float> yPosition, const Optional<float> zPosition)
    {
        SetWorldPosition({ xPosition ? xPosition.value() : position.x, yPosition ? yPosition.value() : position.y, zPosition ? zPosition.value() : position.z });
    }

    void Transform::SetWorldRotation(const Vector3 newRotation)
    {
        if (newRotation == rotation) return;

        dirtyFlag |= TransformDirtyFlag::ROTATION;
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

    void Transform::SetWorldRotation(const Optional<float> xRotation, const Optional<float> yRotation, const Optional<float> zRotation)
    {
        SetWorldRotation({ xRotation ? xRotation.value() : rotation.x, yRotation ? yRotation.value() : rotation.y, zRotation ? zRotation.value() : rotation.z });
    }

    void Transform::SetWorldScale(const Vector3 newScale)
    {
        if (newScale == scale) return;

        dirtyFlag |= TransformDirtyFlag::SCALE;
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

    void Transform::SetWorldScale(const Optional<float> xScale, const Optional<float> yScale, const Optional<float> zScale)
    {
        SetWorldScale({ xScale ? xScale.value() : scale.x, yScale ? yScale.value() : scale.y, zScale ? zScale.value() : scale.z });
    }

    /* --- GETTER  METHODS --- */

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
        if (IsDirty())
        {
            Recalculate(*this);
            for (const auto &child : World::GetComponent<Relationship>(enttEntity).GetEnttChildrenEntities())
            {
                RecalculateChainDeeper(child);
            }
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
            if (!World::GetComponent<Transform>(subChild).IsDirty()) UpdateChainDeeper(subChild);
            else RecalculateChainDeeper(subChild);
        }
    }

    void Transform::RecalculateChainDeeper(const entt::entity currentChild)
    {
        Recalculate(World::GetComponent<Transform>(currentChild));
        for (const auto &subChild : World::GetComponent<Relationship>(currentChild).GetEnttChildrenEntities())
        {
            RecalculateChainDeeper(subChild);
        }
    }

    void Transform::DoCallbacks()
    {
        for (const auto &Callback : OnChangeCallbacks)
        {
            Callback(dirtyFlag);
        }
    }

    void Transform::Recalculate(Transform &tr)
    {
        if (tr.HasParentTransform())
        {
            auto parentTransform = tr.GetParentTransform();
            tr.position = tr.localPosition + parentTransform.position;
            tr.rotation = tr.localRotation + parentTransform.rotation;
            tr.scale = tr.localScale * parentTransform.scale;
            tr.dirtyFlag |= parentTransform.dirtyFlag;
        }
        else
        {
            tr.position = tr.localPosition;
            tr.rotation = tr.localRotation;
            tr.scale = tr.localScale;
        }

        tr.quaternion = Quaternion({glm::radians(-tr.rotation.y), glm::radians(tr.rotation.x), glm::radians(tr.rotation.z) });
        tr.forwardDirection.x = 2 * (tr.quaternion.x * tr.quaternion.z + tr.quaternion.w * tr.quaternion.y);
        tr.forwardDirection.y = 2 * (tr.quaternion.y * tr.quaternion.z - tr.quaternion.w * tr.quaternion.x);
        tr.forwardDirection.z = 1 - 2 * (tr.quaternion.x * tr.quaternion.x + tr.quaternion.y * tr.quaternion.y);

        tr.upDirection.x = 2 * (tr.quaternion.x * tr.quaternion.y - tr.quaternion.w * tr.quaternion.z);
        tr.upDirection.y = 1 - 2 * (tr.quaternion.x * tr.quaternion.x + tr.quaternion.z * tr.quaternion.z);
        tr.upDirection.z = 2 * (tr.quaternion.y * tr.quaternion.z + tr.quaternion.w * tr.quaternion.x);

        tr.DoCallbacks();
        tr.dirtyFlag = TransformDirtyFlag::NONE;
    }

    void Transform::OnDrawUI()
    {
        using namespace Editor;
        GUI::BeginProperties();

        if (GUI::Vector3Property("Position:", localPosition, "Some Tooltip")) dirtyFlag |= TransformDirtyFlag::POSITION;
        if (GUI::Vector3Property("Rotation:", localRotation, "Some Tooltip")) dirtyFlag |= TransformDirtyFlag::ROTATION;
        static float resetValues[3] = { 1.0f, 1.0f, 1.0f };
        if (GUI::Vector3Property("Scale:", localScale, "Some Tooltip", resetValues)) dirtyFlag |= TransformDirtyFlag::SCALE;

        GUI::EndProperties();
    }

    void Transform::PushOnDirtyCallback(const std::function<void(TransformDirtyFlag)> &callback)
    {
        OnChangeCallbacks.push_back(callback);
    }

}