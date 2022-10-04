//
// Created by Nikolay Kanchevski on 28.09.22.
//

#include "Application.h"
#include "../../Engine/Classes/Time.h"
#include "../../Engine/Classes/Cursor.h"
#include "../../Engine/Classes/Input.h"
#include "../../Engine/Classes/File.h"

using namespace Sierra::Core::Rendering;
using namespace Sierra::Engine::Classes;

/* --- POLLING METHODS --- */
void Application::Start()
{
    // Show the window
    Window window = Window("Sierra Engine v1.0.0", false, true, true);
    window.Show();

    VulkanRenderer vulkanRenderer;
    window.SetRenderer(&vulkanRenderer);

    // Initialize utility classes
    StartClasses();

    // Update window until closed
    while (!window.IsClosed()) {
        // Update utility classes
        UpdateClasses();

        // Update window
        window.Update();

        // Set the window title to display current FPS
        window.SetTitle("FPS: " + std::to_string(Time::GetFPS()));
    }
}

void Application::StartClasses()
{
    Input::Start();
}

void Application::UpdateClasses()
{
    Time::Update();
    Cursor::Update();
    Input::Update();
}