//
// Created by Nikolay Kanchevski on 18.01.23.
//

#pragma once


#include "Component.h"

namespace Sierra::Engine::Components
{

    class Transform : public Component
    {

    public:
        /* --- SETTER METHODS --- */
        void SetPosition(Vector3 newPosition);
        void SetPosition(std::optional<float> xPosition, std::optional<float> yPosition, std::optional<float> zPosition);

        void SetRotation(Vector3 newRotation);
        void SetRotation(std::optional<float> xRotation, std::optional<float> yRotation, std::optional<float> zRotation);

        void SetScale(Vector3 newScale);
        void SetScale(std::optional<float> xScale, std::optional<float> yScale, std::optional<float> zScale);

        void SetWorldPosition(Vector3 newPosition);
        void SetWorldPosition(std::optional<float> xPosition, std::optional<float> yPosition, std::optional<float> zPosition);

        void SetWorldRotation(Vector3 newRotation);
        void SetWorldRotation(std::optional<float> xRotation, std::optional<float> yRotation, std::optional<float> zRotation);

        void SetWorldScale(Vector3 newScale);
        void SetWorldScale(std::optional<float> xScale, std::optional<float> yScale, std::optional<float> zScale);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline Vector3 GetPosition() const { return localPosition; }
        [[nodiscard]] inline Vector3 GetRotation() const { return localRotation; }
        [[nodiscard]] inline Vector3 GetScale() const { return localScale; }

        [[nodiscard]] inline Vector3 GetWorldPosition() const { return position; }
        [[nodiscard]] inline Vector3 GetWorldRotation() const { return rotation; }
        [[nodiscard]] inline Vector3 GetWorldScale() const { return scale; }

        [[nodiscard]] Matrix4x4& GetModelMatrix();

        [[nodiscard]] bool HasParentTransform() const;
        [[nodiscard]] Transform& GetParentTransform() const;
        [[nodiscard]] Transform& GetChildTransform(uint childIndex) const;
        [[nodiscard]] Transform& GetOriginTransform() const;

        [[nodiscard]] inline Vector3 GetForwardDirection() const { return forwardDirection; };
        [[nodiscard]] inline Vector3 GetBackDirection() const { return -forwardDirection; };

        [[nodiscard]] inline Vector3 GetLeftDirection() const { return glm::cross(-forwardDirection, upDirection); };
        [[nodiscard]] inline Vector3 GetRightDirection() const { return glm::cross(forwardDirection, upDirection); };

        [[nodiscard]] inline Vector3 GetUpDirection() const { return upDirection; }
        [[nodiscard]] inline Vector3 GetDownDirection() const { return -upDirection; }

        [[nodiscard]] inline Quaternion GetRotationQuaternion() const { return quaternion; };
        [[nodiscard]] inline bool IsDirty() const { return isDirty;  }

        [[nodiscard]] inline Vector3 GetWorldPositionUpInverted() const { return {position.x, -position.y, position.z }; }
        inline void SetWorldPositionUpInverted(const Vector3 newPosition) { SetWorldPosition({ newPosition.x, -newPosition.y, newPosition.z }); };

        /* --- POLLING METHODS --- */
        void UpdateChain();
        void OnDrawUI() override;

        void PushOnDirtyCallback(const Callback &callback);
        void PopOnDirtyCallback();

        /* --- OPERATORS --- */
        Transform() = default;
        Transform(const Transform&) = default;

    private:
        Vector3 position = { 0, 0, 0 };
        Vector3 rotation = { 0, 0, 0 };
        Vector3 scale = { 1, 1, 1 };

        Vector3 localPosition = { 0, 0, 0 };
        Vector3 localRotation = { 0, 0, 0 };
        Vector3 localScale = { 1, 1, 1 };

        bool isDirty = true;
        std::optional<Matrix4x4> modelMatrix;

        Vector3 forwardDirection = { 0, 0, 1 };
        Vector3 upDirection { 0, 1, 0 };
        Quaternion quaternion = glm::identity<Quaternion>();

        std::vector<Callback> OnChangeCallbacks;

        void DoCallbacks();
        void RecalculateOrigin();
        void Recalculate(const Transform &parentTransform);
        static void UpdateChainDeeper(entt::entity currentChild);
        static void RecalculateChainDeeper(entt::entity currentChild, entt::entity parent);

    };

}
