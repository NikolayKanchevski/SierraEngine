//
// Created by Nikolay Kanchevski on 20.12.22.
//

#pragma once

#include "../Engine/Classes/Entity.h"

using Sierra::Engine::Components::UUID;
using Sierra::Engine::Classes::Entity;

namespace Sierra
{
    class EngineCore
    {
    public:
        /* --- POLLING METHODS --- */
        static void Initialize();
        static void Terminate();

        /* --- GETTER METHODS --- */
        [[nodiscard]] static inline Entity& GetSelectedEntity() { return selectedEntity; }
        [[nodiscard]] static inline Vector3 GetMouseHoveredPosition() { return mouseHoveredPosition; }

        /* --- SETTER METHODS --- */
        static void SetSelectedEntity(Entity givenEntity);
        static void SetMouseHoveredPosition(Vector3 givenPosition);

    private:
        static inline Entity selectedEntity;
        static inline Vector3 mouseHoveredPosition = { 0.0f, 0.0f, 0.0f };

    };

}