//
// Created by Nikolay Kanchevski on 8.02.23.
//

#include "WorldManager.h"

namespace Sierra::Engine::Components
{

    /* --- CONSTRUCTORS --- */

    void WorldManager::OnAddComponent()
    {
        // Create default skybox cubemap texture
        skyboxSystem.skyboxCubemap = Cubemap::Create({ .filePaths = {
            "Textures/Skyboxes/Default/skybox_right.png",
            "Textures/Skyboxes/Default/skybox_left.png",
            "Textures/Skyboxes/Default/skybox_top.png",
            "Textures/Skyboxes/Default/skybox_bottom.png",
            "Textures/Skyboxes/Default/skybox_front.png",
            "Textures/Skyboxes/Default/skybox_back.png",
        }, .cubemapType = CUBEMAP_TYPE_SKYBOX });
    }

    /* --- DESTRUCTOR --- */

    void WorldManager::Destroy() const
    {
        skyboxSystem.skyboxCubemap->Destroy();
    }
}
