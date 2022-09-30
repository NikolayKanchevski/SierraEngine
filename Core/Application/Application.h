//
// Created by Nikolay Kanchevski on 28.09.22.
//

#pragma once

#include "../Rendering/Window.h"

using namespace Sierra::Core::Rendering;

class Application
{
public:
    /* --- POLLING METHODS --- */
    void Start();
    static void StartClasses();
    static void UpdateClasses();

private:
    Window window = Window("Sierra Engine v1.0.0", false, true, true);

};