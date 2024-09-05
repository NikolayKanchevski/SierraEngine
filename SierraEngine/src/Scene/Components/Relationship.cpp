//
// Created by Nikolay Kanchevski on 15.05.24.
//

#include "Relationship.h"

namespace SierraEngine
{

    /* --- SETTER METHODS --- */

    void Relationship::SetParent(const EntityID givenParent)
    {
        parent = givenParent;
    }

    void Relationship::RemoveParent()
    {
        parent = 0;
    }

    void Relationship::AddChild(const EntityID child)
    {
        children.push_back(child);
    }

    bool Relationship::RemoveChild(const EntityID child)
    {
        const auto iterator = std::ranges::find(children, child);
        if (iterator != children.end())
        {
            children.erase(iterator);
            return true;
        }

        return false;
    }

}