//
// Created by Nikolay Kanchevski on 18.10.22.
//

#pragma once

#include <glm/vec3.hpp>
#include "Component.h"

namespace Sierra::Engine::Components
{

    class UUID : public Component
    {
    public:
        /* --- CONSTRUCTORS --- */
        UUID();

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint64_t GetValue() const { return value; }

        /* --- OPERATORS --- */
        UUID& operator=(uint32_t givenValue);

        UUID(const UUID&) = default;
        operator uint32_t() const noexcept { return value; }

    private:
        uint64_t value;
    };

    class Tag : public Component
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Tag(std::string givenTag) : tag(std::move(givenTag)) { }

        /* --- PROPERTIES --- */
        std::string tag;

        /* --- OPERATORS --- */
        Tag() = default;
        Tag(const Tag&) = default;
    };

    class Relationship : public Component
    {
    public:
        /* --- CONSTRUCTORS --- */
        inline explicit Relationship(entt::entity givenSelf) : self(givenSelf) { }

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline entt::entity& GetEnttParentEntity() { return parent; }
        [[nodiscard]] inline std::vector<entt::entity> GetEnttChildrenEntities() const { return children; }

        /* --- SETTER METHODS --- */
        void SetParent(entt::entity givenParent);

        /* --- OPERATORS --- */
        Relationship() = default;
        Relationship(const Relationship&) = default;

    private:
        entt::entity parent = entt::null;
        entt::entity self = entt::null;
        std::vector<entt::entity> children {};
    };

    class Transform : public Component
    {
    public:
        /* --- PROPERTIES --- */
        glm::vec3 position = {0, 0, 0 };
        glm::vec3 rotation = { 0, 0, 0 };
        glm::vec3 scale = { 1, 1, 1 };

        /* --- POLLING METHODS --- */
        void DrawUI() override;

        /* --- OPERATORS --- */
        Transform() = default;
        Transform(const Transform&) = default;
    };
}