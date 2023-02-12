//
// Created by Nikolay Kanchevski on 8.02.23.
//

#pragma once

#include "Component.h"

#include "../../Core/Rendering/Vulkan/Abstractions/Cubemap.h"

namespace Sierra::Engine::Components
{
    using namespace Core::Rendering::Vulkan::Abstractions;

    class WorldManager : public Component
    {
    private:
        struct SkyboxSystem
        {
            UniquePtr<Cubemap> skyboxCubemap;
            Vector3 skyboxRotation = { 0.0f, 0.0f, 0.0f };
            Vector4 skyboxTint = { 1.0f, 1.0f, 1.0f, 1.0f };
        };

    public:
        /* --- POLLING METHODS --- */
        void OnAddComponent() override;

        /* --- GETTER METHODS --- */
        inline SkyboxSystem& GetSkyboxSystem() { return skyboxSystem; }

        /* --- DESTRUCTOR --- */
        void Destroy() const override;
    private:
        SkyboxSystem skyboxSystem = {};

    };

}