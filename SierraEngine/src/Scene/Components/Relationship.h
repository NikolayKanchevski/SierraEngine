//
// Created by Nikolay Kanchevski on 15.05.24.
//

#pragma once

#include "../Component.h"

namespace SierraEngine
{

    /* --- TYPE DEFINITIONS --- */
    using EntityID = Sierra::UUID<ENTT_ID_TYPE>;

    class SIERRA_ENGINE_API Relationship final : public Component
    {
    public:
        /* --- CONSTRUCTORS --- */
        Relationship() = default;

        /* --- SETTER METHODS --- */
        void SetParent(EntityID parent);
        void RemoveParent();

        void AddChild(EntityID child);
        bool RemoveChild(EntityID child);

        /* --- GETTER METHODS --- */
        [[nodiscard]] EntityID GetParentID() const { return parent; }
        [[nodiscard]] std::span<const EntityID> GetChildrenIDs() const { return children; }

        /* --- TYPE DATA --- */
        [[nodiscard]] constexpr static std::string_view GetName() { return "Relationship"; }

        /* --- DESTRUCTOR --- */
        ~Relationship() = default;
    
    private:
        EntityID parent = 0;
        std::vector<EntityID> children = { };

    };

}
