//
// Created by Nikolay Kanchevski on 18.01.23.
//

#pragma once

#include "Component.h"

namespace Sierra::Engine
{

    enum class TransformDirtyFlag
    {
        NONE       = 0b00000000,
        POSITION   = 0b00000001,
        ROTATION   = 0b00000010,
        SCALE      = 0b00000100,
    };

    DEFINE_ENUM_FLAG_OPERATORS(TransformDirtyFlag);

    class Transform : public Component
    {
    public:
        /* --- CONSTRUCTORS --- */
        Transform() = default;

        /* --- SETTER METHODS --- */
        void SetPosition(Vector3 newPosition);
        void SetPosition(Optional<float> xPosition, Optional<float> yPosition, Optional<float> zPosition);

        void SetRotation(Vector3 newRotation);
        void SetRotation(Optional<float> xRotation, Optional<float> yRotation, Optional<float> zRotation);

        void SetScale(Vector3 newScale);
        void SetScale(Optional<float> xScale, Optional<float> yScale, Optional<float> zScale);

        void SetWorldPosition(Vector3 newPosition);
        void SetWorldPosition(Optional<float> xPosition, Optional<float> yPosition, Optional<float> zPosition);

        void SetWorldRotation(Vector3 newRotation);
        void SetWorldRotation(Optional<float> xRotation, Optional<float> yRotation, Optional<float> zRotation);

        void SetWorldScale(Vector3 newScale);
        void SetWorldScale(Optional<float> xScale, Optional<float> yScale, Optional<float> zScale);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline Vector3 GetPosition() const { return localPosition; }
        [[nodiscard]] inline Vector3 GetRotation() const { return localRotation; }
        [[nodiscard]] inline Vector3 GetScale() const { return localScale; }

        [[nodiscard]] inline Vector3 GetWorldPosition() const { return position; }
        [[nodiscard]] inline Vector3 GetWorldRotation() const { return rotation; }
        [[nodiscard]] inline Vector3 GetWorldScale() const { return scale; }

        [[nodiscard]] bool HasParentTransform() const;
        [[nodiscard]] Transform& GetParentTransform() const;
        [[nodiscard]] Transform& GetChildTransform(uint32 childIndex) const;

        [[nodiscard]] inline bool IsDirty() const { return IS_FLAG_PRESENT(dirtyFlag, TransformDirtyFlag::POSITION) || IS_FLAG_PRESENT(dirtyFlag, TransformDirtyFlag::ROTATION) || IS_FLAG_PRESENT(dirtyFlag, TransformDirtyFlag::SCALE);  }

        /* --- POLLING METHODS --- */
        void UpdateChain();
        void OnDrawUI() override;
        void PushOnDirtyCallback(const std::function<void(TransformDirtyFlag)> &callback);

    private:
        Vector3 position = { 0, 0, 0 };
        Vector3 rotation = { 0, 0, 0 };
        Vector3 scale = { 1, 1, 1 };

        Vector3 localPosition = { 0, 0, 0 };
        Vector3 localRotation = { 0, 0, 0 };
        Vector3 localScale = { 1, 1, 1 };

        TransformDirtyFlag dirtyFlag = TransformDirtyFlag::POSITION | TransformDirtyFlag::ROTATION | TransformDirtyFlag::SCALE;

        std::vector<std::function<void(TransformDirtyFlag)>> OnChangeCallbacks;

        void DoCallbacks();
        static void Recalculate(Transform &transform);
        static void UpdateChainDeeper(const Entity &currentChild);
        static void RecalculateChainDeeper(const Entity &currentChild);

    };

}
