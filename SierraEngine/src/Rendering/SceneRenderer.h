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
        const RenderingContext& renderingContext;
    };

    /* --- TYPE DEFINITIONS --- */
    using RenderTargetID = Sierra::Handle<uint32>;

    struct SceneRenderInfo
    {
        RenderTargetID renderTarget = 0;
        uint32 width = 0;
        uint32 height = 0;

        const Scene& scene;
        const Camera& camera;
        const Transform& eye;
    };

    class SIERRA_ENGINE_API SceneRenderer final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit SceneRenderer(const SceneRendererCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] RenderTargetID CreateRenderTarget();
        bool DestroyRenderTarget(RenderTargetID ID);

        [[nodiscard]] const Sierra::Image& Render(Sierra::CommandBuffer& commandBuffer, const SceneRenderInfo& renderInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] const Sierra::Image* GetRenderTargetImage(RenderTargetID ID);

        /* --- COPY SEMANTICS --- */
        SceneRenderer(const SceneRenderer&) = delete;
        SceneRenderer& operator=(const SceneRenderer&) = delete;

        /* --- MOVE SEMANTICS --- */
        SceneRenderer(SceneRenderer&&) = default;
        SceneRenderer& operator=(SceneRenderer&&) = default;

        /* --- DESTRUCTOR --- */
        ~SceneRenderer() noexcept = default;

    private:
        const RenderingContext* renderingContext;

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

            [[nodiscard]] bool IsValid() const noexcept { return colorImage != nullptr && depthImage != nullptr && framebuffer != nullptr; }
        };

        Sierra::HandleManager<RenderTargetID, RenderTarget> renderTargets = { };
        void CreateRenderTarget(RenderTarget& renderTarget, uint32 width, uint32 height) const;
        void DestroyRenderTarget(RenderTarget& renderTarget) const;

    };

}