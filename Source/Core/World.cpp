//
// Created by Nikolay Kanchevski on 10.10.22.
//

#include "../Engine/Classes/Time.h"
#include "../Engine/Classes/Input.h"
#include "../Engine/Classes/File.h"
#include "../Engine/Classes/Cursor.h"
#include "../Engine/Classes/Entity.h"
#include "../Engine/Handlers/Project.h"
#include "../Engine/Classes/Discord.h"
#include "../Engine/Handlers/Assets/AssetManager.h"
#include "../Engine/Components/Transform.h"
#include "../Engine/Classes/SystemInformation.h"
#include "Rendering/Bases/VK.h"

namespace Sierra::Engine
{

    /* --- POLLING METHODS --- */

    void World::Initialize()
    {
        // Initialize engine core
        File::Initialize();
        Project::Initialize();

        // Initialize rendering resources
        glfwInit();
        Rendering::VK::Initialize();
        AssetManager::Initialize();

        // Initialize utilities
        Discord::Start(SR_DISCORD_APPLICATION_KEY, [] {
            ASSERT_INFO("Discord user picked: {0}", Discord::GetUser().GetUsername());
            Discord::GetActivity()
                .SetDetails("Creating AAA games!")
                .SetLargeImageHoverText("Sierra Engine v1.0.0")
                .SetLargeImage("https://cdn.discordapp.com/app-icons/1118235723917705230/1cd62012759c1217be085bc595edc892.png?size=512")
                .SetSmallImageHoverText("Vulkan API v1.3")
                .SetSmallImage("https://pbs.twimg.com/profile_images/1494006370735067137/2uOq5953_400x400.jpg")
                .SetType(discord::ActivityType::Playing);
        });
        Input::Initialize();
        Cursor::Initialize();
        SystemInformation::Initialize();
    }

    void World::Update()
    {
        // Update utility classes
        Time::Update();
        Discord::Update();
        Cursor::Update();
        Input::Update();

        // Recursively update all entities' Transform
        for (const auto &entity : originEntities)
        {
            Entity(entity).GetComponent<Transform>().UpdateChain();
        }
    }

    void World::Shutdown()
    {
        Discord::Destroy();
        SystemInformation::Shutdown();
    }

    /* --- SETTER METHODS --- */

    void World::DestroyEntity(const entt::entity entity)
    {
        enttRegistry.destroy(entity);
    }

}
