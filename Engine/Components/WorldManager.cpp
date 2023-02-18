//
// Created by Nikolay Kanchevski on 8.02.23.
//

#include "WorldManager.h"

#include "../Classes/File.h"

using namespace Sierra::Engine::Classes;

namespace Sierra::Engine::Components
{

    /* --- CONSTRUCTORS --- */

    void WorldManager::OnAddComponent()
    {
        // Create default skybox cubemap texture
        skyboxSystem.skyboxCubemap = Cubemap::Create({ .filePaths = {
            File::OUTPUT_FOLDER_PATH + "Textures/Skyboxes/Default/skybox_right.png",
            File::OUTPUT_FOLDER_PATH + "Textures/Skyboxes/Default/skybox_left.png",
            File::OUTPUT_FOLDER_PATH + "Textures/Skyboxes/Default/skybox_top.png",
            File::OUTPUT_FOLDER_PATH + "Textures/Skyboxes/Default/skybox_bottom.png",
            File::OUTPUT_FOLDER_PATH + "Textures/Skyboxes/Default/skybox_front.png",
            File::OUTPUT_FOLDER_PATH + "Textures/Skyboxes/Default/skybox_back.png",
        }, .cubemapType = CUBEMAP_TYPE_SKYBOX });
    }

    /* --- DESTRUCTOR --- */

    void WorldManager::Destroy() const
    {
        skyboxSystem.skyboxCubemap->Destroy();
    }
}
