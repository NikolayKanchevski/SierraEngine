//
// Created by Nikolay Kanchevski on 28.09.22.
//

#include <iostream>
#include <fstream>
#include "Application.h"
#include "../../Engine/Classes/Time.h"
#include "../../Engine/Classes/Cursor.h"
#include "../../Engine/Classes/Input.h"
#include "../../Engine/Classes/File.h"

#include <stb_image.h>

using namespace Sierra::Core::Rendering;
using namespace Sierra::Engine::Classes;

/* --- POLLING METHODS --- */
void Application::Start()
{
    // Show the window
    window.Show();

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