//
// Created by Nikolay Kanchevski on 15.05.24.
//

#include "Relationship.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    Relationship::Relationship(const entt::entity parent)
    {
        SetParent(parent);
    }

    /* --- SETTER METHODS --- */

    void Relationship::SetParent(const entt::entity parent)
    {
        this->parent = parent;
    }

    void Relationship::RemoveParent()
    {
        parent = entt::null;
    }


    void Relationship::AddChild(const entt::entity child)
    {
        children.push_back(child);
    }

    void Relationship::RemoveChild(const entt::entity child)
    {
        children.erase(std::ranges::remove(children, child).begin());
    }

}