//
// Created by Nikolay Kanchevski on 15.05.24.
//

#pragma once

#include "../Component.h"

#include "../EntityID.h"

namespace SierraEngine
{

    class SIERRA_ENGINE_API Relationship final : public Component
    {
    public:
        /* --- CONSTRUCTORS --- */
        Relationship() noexcept = default;

        /* --- SETTER METHODS --- */
        void SetParent(EntityID parent) noexcept;
        void RemoveParent() noexcept;

        void AddChild(EntityID child) noexcept;
        bool RemoveChild(EntityID child) noexcept;

        /* --- GETTER METHODS --- */
        [[nodiscard]] EntityID GetParentID() const noexcept { return parent; }
        [[nodiscard]] std::span<const EntityID> GetChildrenIDs() const noexcept { return children; }

        /* --- TYPE DATA --- */
        [[nodiscard]] constexpr static std::string_view GetName() { return "Relationship"; }

        /* --- COPY SEMANTICS --- */
        Relationship(const Relationship&) = delete;
        Relationship& operator=(const Relationship&) = delete;

        /* --- MOVE SEMANTICS --- */
        Relationship(Relationship&&) noexcept = default;
        Relationship& operator=(Relationship&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~Relationship() noexcept = default;
    
    private:
        EntityID parent = 0;
        std::vector<EntityID> children = { };

    };

}
