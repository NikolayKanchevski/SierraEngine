//
// Created by Nikolay Kanchevski on 15.05.24.
//

#include "Relationship.h"

namespace SierraEngine
{

    /* --- SETTER METHODS --- */

    void Relationship::SetParent(const EntityID givenParent) noexcept
    {
        parent = givenParent;
    }

    void Relationship::RemoveParent() noexcept
    {
        parent = 0;
    }

    void Relationship::AddChild(const EntityID child) noexcept
    {
        children.push_back(child);
    }

    bool Relationship::RemoveChild(const EntityID child) noexcept
    {
        const auto iterator = std::find(children.begin(), children.end(), child);
        if (iterator != children.end())
        {
            children.erase(iterator);
            return true;
        }

        return false;
    }

}