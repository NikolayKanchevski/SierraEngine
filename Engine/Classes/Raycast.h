//
// Created by Nikolay Kanchevski on 29.06.23.
//

#pragma once

#include "Entity.h"

namespace Sierra::Engine
{

    class Raycast
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] inline static bool IsHoveringEntity() { return !hoveredEntity.IsNull(); }
        [[nodiscard]] inline static Entity GetHoveredEntity() { return hoveredEntity; }
        [[nodiscard]] inline static Vector3 GetHoveredWorldPosition() { return hoveredWorldPosition; }

        /* --- CALLBACKS --- */
        static void HoveredEntityCallback(Entity entity);
        static void HoveredWorldPositionCallback(Vector3 position);

    private:
        static inline Entity hoveredEntity = Entity::Null;
        static inline Vector3 hoveredWorldPosition = { 0.0f, 0.0f, 0.0f };

    };

}