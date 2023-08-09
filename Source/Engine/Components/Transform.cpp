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
        Entity parentEntity = GetComponent<Relationship>().GetParent();
        return !parentEntity.IsNull();
    }

    Transform& Transform::GetParentTransform() const
    {
        return GetComponent<Relationship>().GetParent().GetComponent<Transform>();
    }

    Transform& Transform::GetChildTransform(const uint32 childIndex) const
    {
        return GetComponent<Relationship>().GetChildren()[childIndex].GetComponent<Transform>();
    }

    /* --- POLLING METHODS --- */

    void Transform::UpdateChain()
    {
        if (IsDirty())
        {
            Recalculate(*this);
            for (const auto &child : GetComponent<Relationship>().GetChildren())
            {
                RecalculateChainDeeper(child);
            }
        }
        else
        {
            UpdateChainDeeper(GetEntity());
        }
    }

    void Transform::UpdateChainDeeper(const Entity &currentChild)
    {
        for (const auto &subChild : currentChild.GetComponent<Relationship>().GetChildren())
        {
            if (!subChild.GetComponent<Transform>().IsDirty()) UpdateChainDeeper(subChild);
            else RecalculateChainDeeper(subChild);
        }
    }

    void Transform::RecalculateChainDeeper(const Entity &currentChild)
    {
        Recalculate(currentChild.GetComponent<Transform>());
        for (const auto &subChild : currentChild.GetComponent<Relationship>().GetChildren())
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

    void Transform::Recalculate(Transform &transform)
    {
        if (transform.HasParentTransform())
        {
            auto parentTransform = transform.GetParentTransform();
            transform.position = transform.localPosition + parentTransform.position;
            transform.rotation = transform.localRotation + parentTransform.rotation;
            transform.scale = transform.localScale * parentTransform.scale;
        }
        else
        {
            transform.position = transform.localPosition;
            transform.rotation = transform.localRotation;
            transform.scale = transform.localScale;
        }

        transform.DoCallbacks();
        transform.dirtyFlag = TransformDirtyFlag::NONE;
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