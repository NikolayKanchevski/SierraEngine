//
// Created by Nikolay Kanchevski on 13.05.24.
//

#pragma once

#include "../Editor/Editor.h"
#include "../Rendering/EditorSurface.h"

namespace SierraEngine
{

    class EditorApplication final : public Application
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit EditorApplication(const ApplicationCreateInfo &createInfo);

        /* --- DESTRUCTOR --- */
        ~EditorApplication() override;

    private:
        void Start() override { }
        bool Update(const Sierra::TimeStep &timeStep) override;

        EditorSurface surface;
        ThreadPool threadPool;

        std::vector<std::unique_ptr<Sierra::CommandBuffer>> commandBuffers = { };

        Scene scene;
        std::unique_ptr<Editor> editor = nullptr;

    };

}
