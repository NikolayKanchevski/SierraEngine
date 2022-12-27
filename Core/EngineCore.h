//
// Created by Nikolay Kanchevski on 20.12.22.
//

#pragma once

#include <GLFW/glfw3.h>

#include "../Engine/Components/InternalComponents.h"
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

        /* --- SETTER METHODS --- */
        static void SetSelectedEntity(Entity givenEntity);

    private:
        static inline Entity selectedEntity;

    };

}