//
// Created by Nikolay Kanchevski on 10.10.22.
//

#include "../Engine/Classes/Time.h"
#include "../Engine/Classes/Cursor.h"
#include "../Engine/Classes/Input.h"
#include "../Engine/Classes/Entity.h"
#include "../Engine/Components/Transform.h"

using namespace Sierra::Engine::Classes;

namespace Sierra::Core
{
    /* --- POLLING METHODS --- */

    void World::Start()
    {
        Input::Start();
        Cursor::Start();
    }

    void World::Prepare()
    {
        Time::Update();
        Cursor::Update();
        Input::Update();

        for (const auto &entityData : originEntities)
        {
            Entity entity = Entity(entityData.second);
            entity.GetComponent<Transform>().UpdateChain();
        }
    }

    void World::Update()
    {

    }

}
