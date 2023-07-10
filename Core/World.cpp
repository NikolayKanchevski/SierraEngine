//
// Created by Nikolay Kanchevski on 10.10.22.
//

#include "Rendering/Vulkan/VK.h"
#include "../Engine/Classes/Time.h"
#include "../Engine/Classes/File.h"
#include "../Engine/Classes/Input.h"
#include "../Engine/Classes/Entity.h"
#include "../Engine/Classes/Cursor.h"
#include "../Engine/Components/Model.h"
#include "../Engine/Components/Transform.h"
#include "../Engine/Classes/SystemInformation.h"

namespace Sierra::Engine
{
    Entity World::selectedEntity = Entity::Null;

    /* --- POLLING METHODS --- */

    void World::Start()
    {
        // Initialize engine resources
        glfwInit();
        Input::Start();
        Cursor::Start();
        File::Start();
        SystemInformation::Start();

        // Initialize rendering resources
        Rendering::VK::Initialize();
    }

    void World::Prepare()
    {
        Time::Update();
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
    }

    /* --- SETTER METHODS --- */

    void World::SetSelectedEntity(const Entity entity)
    {
        selectedEntity = entity;
    }

    void World::DestroyEntity(const Entity entity)
    {
        enttRegistry->destroy(entity);
    }

    /* --- GETTER METHODS --- */

    Entity World::GetSelectedEntity()
    {
        return selectedEntity;
    }

}
