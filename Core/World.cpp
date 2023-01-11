//
// Created by Nikolay Kanchevski on 10.10.22.
//

#include "World.h"

#include <glm/gtc/matrix_transform.hpp>

#include "../Engine/Classes/Time.h"
#include "../Engine/Classes/Cursor.h"
#include "../Engine/Classes/Input.h"

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
    }

    void World::Update()
    {

    }

}
