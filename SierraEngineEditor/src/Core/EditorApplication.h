//
// Created by Nikolay Kanchevski on 13.05.24.
//

#pragma once

#include "../Assets/EditorAssetManager.h"
#include "../Rendering/EditorRenderer.h"
#include "../Editor/Editor.h"

namespace SierraEngine
{

    class EditorApplication final : public Application
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit EditorApplication(const ApplicationCreateInfo& createInfo);

        /* --- DESTRUCTOR --- */
        ~EditorApplication() override;

    private:
        bool Update() override;

        FrameLimiter frameLimiter;
        ThreadPool threadPool;

        Surface surface;
        std::unique_ptr<Sierra::ResourceTable> resourceTable;
        std::vector<std::unique_ptr<Sierra::CommandBuffer>> commandBuffers = { };

        EditorAssetManager assetManager;
        Scene scene;

        TriangleRenderer triangleRenderer;

        Editor editor;
        std::unique_ptr<EditorRenderer> editorRenderer;

    };

}
