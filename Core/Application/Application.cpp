//
// Created by Nikolay Kanchevski on 28.09.22.
//

#include "Application.h"
#include "../../Core/Rendering/Vulkan/Renderer/VulkanRenderer.h"
#include "../../Engine/Classes/Time.h"
#include "../../Engine/Classes/Cursor.h"
#include "../../Engine/Classes/Input.h"
#include "../../Engine/Classes/File.h"

using namespace Sierra::Core::Rendering;
using namespace Sierra::Engine::Classes;

/* --- POLLING METHODS --- */
void Application::Start()
{
    // Create the renderer
    VulkanRenderer renderer("Sierra Engine v1.0.0", false, true, true);
    renderer.Start();

    // Get a reference to the window of the renderer
    Window &window = renderer.GetWindow();

    // Initialize utility classes
    StartClasses();

    // Update window until closed
    while (!renderer.IsActive()) {
        // Update utility classes
        UpdateClasses();

        // Update window
        renderer.Update();

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