//
// Created by Nikolay Kanchevski on 29.06.23.
//

#pragma once

#include "Entity.h"

namespace Sierra::Engine::Raycast
{

    /* --- SETTER METHODS --- */
    void SetHoveredEntity(const Entity &entity);
    void SetHoveredWorldPosition(const Vector3 &position);

    /* --- GETTER METHODS --- */
    [[nodiscard]] bool IsHoveringEntity();
    [[nodiscard]] Entity GetHoveredEntity();
    [[nodiscard]] Vector3 GetHoveredWorldPosition();

}