//
// Created by Nikolay Kanchevski on 28.09.22.
//

#include "Application.h"
#include "../../Engine/Classes/Time.h"
#include "../../Engine/Classes/Cursor.h"

using namespace Sierra::Core::Rendering;
using namespace Sierra::Engine::Classes;

/* --- POLLING METHODS --- */
void Application::Start()
{
    // Show the window
    window.Show();

    // Update window until closed
    while (!window.IsClosed()) {
        UpdateClasses();

        window.Update();

        window.SetTitle("FPS: " + std::to_string(Time::GetFPS()));
    }
}

void Application::UpdateClasses()
{
    Time::Update();
    Cursor::Update();
}