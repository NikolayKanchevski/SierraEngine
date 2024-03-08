//
// Created by Nikolay Kanchevski on 18.02.24.
//

#pragma once

#include "ThreadPool.hpp"
#include "../Assets/AssetManager.h"

#if SR_EDITOR_APPLICATION
    #include "../Assets/Platform/Editor/EditorAssetManager.h"
    using ApplicationAssetManager = SierraEngine::EditorAssetManager;
#elif SR_STANDALONE_APPLICATION
    #error "ERR"
#endif

namespace SierraEngine
{

    class Application final : public Sierra::Application
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Application(const Sierra::ApplicationCreateInfo &createInfo);

        /* --- DESTRUCTOR --- */
        ~Application() override = default;

    private:
        void Start() override;
        bool Update(const Sierra::TimeStep &timeStep) override;

        ThreadPool threadPool;
        ApplicationAssetManager assetManager;

        std::unique_ptr<Sierra::Window> window = nullptr;
        std::unique_ptr<Sierra::Swapchain> swapchain = nullptr;
        std::unique_ptr<Sierra::RenderPass> renderPass = nullptr;

        std::unique_ptr<Sierra::Shader> vertexShader = nullptr;
        std::unique_ptr<Sierra::Shader> fragmentShader = nullptr;

        std::unique_ptr<Sierra::PipelineLayout> pipelineLayout = nullptr;
        std::unique_ptr<Sierra::GraphicsPipeline> graphicsPipeline = nullptr;
        std::vector<std::unique_ptr<Sierra::CommandBuffer>> commandBuffers;

    };

}