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
        explicit EditorApplication(const ApplicationCreateInfo& createInfo);

        /* --- COPY SEMANTICS --- */
        EditorApplication(const EditorApplication&) = delete;
        EditorApplication& operator=(const EditorApplication&) = delete;

        /* --- MOVE SEMANTICS --- */
        EditorApplication(EditorApplication&&) = delete;
        EditorApplication& operator=(EditorApplication&&) = delete;

        /* --- DESTRUCTOR --- */
        ~EditorApplication() noexcept override;

    private:
        bool Update() override;

        FrameLimiter frameLimiter;
        ThreadPool threadPool;

        std::unique_ptr<Sierra::Device> device = nullptr;
        std::unique_ptr<Sierra::Queue> queue = nullptr;

        std::unique_ptr<Sierra::ResourceTable> resourceTable = nullptr;
        std::vector<std::unique_ptr<Sierra::CommandBuffer>> commandBuffers = { };

        ArenaAllocator arenaAllocator;
        SceneRenderer sceneRenderer;

        Editor editor;
        std::optional<EditorSurface> editorSurface = std::nullopt;

        Scene scene;

    };

}
