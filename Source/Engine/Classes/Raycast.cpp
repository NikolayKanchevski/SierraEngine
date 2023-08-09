//
// Created by Nikolay Kanchevski on 29.06.23.
//

#include "Raycast.h"

namespace Sierra::Engine
{
    /* --- PROPERTIES --- */

    Entity hoveredEntity = Entity::Null;
    Vector3 hoveredWorldPosition = { 0.0f, 0.0f, 0.0f };

    /* --- SETTER METHODS --- */

    void Raycast::SetHoveredEntity(const Entity &entity)
    {
        hoveredEntity = entity;
    }

    void Raycast::SetHoveredWorldPosition(const Vector3 &position)
    {
        hoveredWorldPosition = position;
    }

    /* --- GETTER METHODS --- */

    bool Raycast::IsHoveringEntity()                { return hoveredEntity != Entity::Null; }
    Entity Raycast::GetHoveredEntity()              { return hoveredEntity; }
    Vector3 Raycast::GetHoveredWorldPosition()      { return hoveredWorldPosition; }
}