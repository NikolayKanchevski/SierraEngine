//
// Created by Nikolay Kanchevski on 10.10.22.
//

#include "Rendering/Vulkan/VK.h"
#include "../Engine/Classes/Time.h"
#include "../Engine/Classes/File.h"
#include "../Engine/Classes/Input.h"
#include "../Engine/Classes/Entity.h"
#include "../Engine/Classes/Cursor.h"
#include "../Engine/Classes/Discord.h"
#include "../Engine/Components/Model.h"
#include "../Engine/Components/Transform.h"
#include "../Engine/Classes/SystemInformation.h"

namespace Sierra::Engine
{

    /* --- POLLING METHODS --- */

    void World::Start()
    {
        Discord::Start(SR_DISCORD_APPLICATION_KEY, [] {
            ASSERT_INFO_FORMATTED("Discord user picked: {0}", Discord::GetUser().GetUsername());
            Discord::GetActivity()
                .SetDetails("Creating AAA games!")
                .SetLargeImageHoverText("Sierra Engine v1.0.0")
                .SetLargeImage("https://cdn.discordapp.com/app-icons/1118235723917705230/1cd62012759c1217be085bc595edc892.png?size=512")
                .SetSmallImageHoverText("Vulkan API v1.3")
                .SetSmallImage("https://pbs.twimg.com/profile_images/1494006370735067137/2uOq5953_400x400.jpg")
                .SetType(discord::ActivityType::Playing);
        });

        // Initialize engine resources
        glfwInit();
        Input::Start();
        Cursor::Start();
        File::Start();
        Rendering::VK::Initialize();
        SystemInformation::Start();
    }

    void World::Prepare()
    {
        Time::Update();
        Discord::Update();
        Cursor::Update();
        Input::Update();
    }

    void World::Update()
    {
        for (const auto &entityData : originEntities)
        {
            Entity entity = Entity(entityData.second);
            entity.GetComponent<Transform>().UpdateChain();
        }
    }

    void World::Shutdown()
    {
        Model::DisposePool();
        Rendering::Texture::DisposePool();
        Rendering::Texture::DestroyDefaultTextures();

        SystemInformation::Shutdown();
        glfwTerminate();

        Discord::Destroy();
    }

    /* --- SETTER METHODS --- */

    void World::SetSelectedEntity(const Entity &entity)
    {
        selectedEntity = entity;
    }

    void World::DestroyEntity(const Entity &entity)
    {
        enttRegistry.destroy(entity);
    }

    /* --- GETTER METHODS --- */

    Entity World::GetSelectedEntity()
    {
        return selectedEntity;
    }

}
