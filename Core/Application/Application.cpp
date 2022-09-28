//
// Created by Nikolay Kanchevski on 28.09.22.
//

#include "Application.h"

using namespace Sierra::Core::Rendering;

/* --- POLLING METHODS --- */
void Application::Start()
{
    // Show the window
    window.Show();

    // Update window until closed
    while (!window.IsClosed()) {
        window.Update();
    }
}