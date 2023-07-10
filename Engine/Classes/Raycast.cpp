//
// Created by Nikolay Kanchevski on 29.06.23.
//

#include "Raycast.h"

namespace Sierra::Engine
{

    void Raycast::HoveredEntityCallback(const Entity entity)
    {
        hoveredEntity = entity;
    }

    void Raycast::HoveredWorldPositionCallback(const Vector3 position)
    {
        hoveredWorldPosition = position;
    }
}