//
// Created by Nikolay Kanchevski on 10.10.22.
//

#include "../Engine/Classes/Time.h"
#include "../Engine/Classes/Cursor.h"
#include "../Engine/Classes/Input.h"
#include "../Engine/Classes/Entity.h"
#include "../Engine/Components/Transform.h"
#include "../Engine/Components/WorldManager.h"
#include "../Engine/Classes/SystemInformation.h"

using namespace Sierra::Engine::Classes;

namespace Sierra::Core
{
    /* --- POLLING METHODS --- */

    void World::Start()
    {
        // Create scene entity
        worldManagerEntity = World::RegisterEntity();
        World::AddComponent<WorldManager>(worldManagerEntity);

        Input::Start();
        Cursor::Start();
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
        GetManager().GetSkyboxSystem().skyboxCubemap->Destroy();
        SystemInformation::Shutdown();
    }

    /* --- GETTER METHODS --- */
    WorldManager& World::GetManager()
    {
        return GetComponent<WorldManager>(worldManagerEntity);
    }

}
