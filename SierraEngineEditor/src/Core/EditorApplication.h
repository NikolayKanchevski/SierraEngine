//
// Created by Nikolay Kanchevski on 13.05.24.
//

#pragma once

#include "Project.h"
#include "../Assets/EditorAssetManager.h"

#include "../Editor/Editor.h"
#include "../Platform/EditorPlatformContext.h"
#include "../Rendering/EditorSurface.h"

namespace SierraEngine
{

    struct EditorApplicationCreateInfo
    {
        const std::filesystem::path& projectDirectoryPath;
        Sierra::ApplicationSettings settings = { };
    };

    class EditorApplication final : public Sierra::Application
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit EditorApplication(const EditorApplicationCreateInfo& createInfo);

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
        std::unique_ptr<EditorPlatformContext> platformContext;

        FrameLimiter frameLimiter;
        Project project;

        RenderingContext renderingContext;
        std::unique_ptr<Sierra::Queue> queue = nullptr;
        std::vector<std::unique_ptr<Sierra::CommandBuffer>> commandBuffers = { };

        std::optional<EditorSurface> surface;
        std::shared_ptr<SceneRenderer> sceneRenderer = nullptr;

        ArenaAllocator arenaAllocator;
        EditorAssetManager assetManager;
        Editor editor;

    };

}
