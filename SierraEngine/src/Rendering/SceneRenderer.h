//
// Created by Nikolay Kanchevski on 29.10.24.
//

#pragma once

#include "../Scene/Scene.h"
#include "../Scene/Components/Camera.h"
#include "../Scene/Components/Transform.h"

namespace SierraEngine
{

    struct SceneRendererCreateInfo
    {
        const Sierra::Device& device;
    };

    struct RenderTargetCreateInfo
    {
        uint32 width = 0;
        uint32 height = 0;
    };

    struct SceneRenderInfo
    {
        uint32 width = 0;
        uint32 height = 0;

        const Scene& scene;
        const Camera& camera;
        const Transform& eye;
    };

    /* --- TYPE DEFINITIONS --- */
    using RenderTargetID = Sierra::Handle<uint32>;

    class SIERRA_ENGINE_API SceneRenderer final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit SceneRenderer(const SceneRendererCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] RenderTargetID CreateRenderTarget(const RenderTargetCreateInfo& createInfo);
        bool DestroyRenderTarget(RenderTargetID ID);

        void Render(Sierra::CommandBuffer& commandBuffer, RenderTargetID renderTargetID, const SceneRenderInfo& renderInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] const Sierra::Image* GetRenderTargetImage(const RenderTargetID ID);

        /* --- COPY SEMANTICS --- */
        SceneRenderer(const SceneRenderer&) = delete;
        SceneRenderer& operator=(const SceneRenderer&) = delete;

        /* --- MOVE SEMANTICS --- */
        SceneRenderer(SceneRenderer&&) = delete;
        SceneRenderer& operator=(SceneRenderer&&) = delete;

        /* --- DESTRUCTOR --- */
        ~SceneRenderer() noexcept = default;

    private:
        const Sierra::Device* device;

        std::unique_ptr<Sierra::RenderPass> renderPass = nullptr;
        std::unique_ptr<Sierra::Shader> vertexShader = nullptr;
        std::unique_ptr<Sierra::Shader> fragmentShader = nullptr;
        std::unique_ptr<Sierra::GraphicsPipeline> graphicsPipeline = { };

        Sierra::ImageFormat colorFormat = Sierra::ImageFormat::Undefined;
        Sierra::ImageFormat depthFormat = Sierra::ImageFormat::Undefined;

        struct PushConstant
        {
            Matrix4x4 view = Matrix4x4(1.0f);
            Matrix4x4 projection = Matrix4x4(1.0f);
        };

        struct RenderTarget
        {
            std::unique_ptr<Sierra::Image> colorImage = nullptr;
            std::unique_ptr<Sierra::Image> depthImage = nullptr;
            std::unique_ptr<Sierra::Framebuffer> framebuffer = nullptr;
        };

        Sierra::IndexPool<RenderTargetID> renderTargetIndexPool = { };
        std::vector<std::optional<RenderTarget>> renderTargets = { };
        void ResizeRenderTarget(RenderTargetID ID, uint32 width, uint32 height);

    };

}