//
// Created by Nikolay Kanchevski on 29.10.24.
//

#pragma once

#include "../Scene/Scene.h"
#include "../Scene/Components/Camera.h"
#include "../Scene/Components/Transform.h"
#include "../Rendering/RenderingContext.h"

namespace SierraEngine
{

    struct SceneRendererCreateInfo
    {
        const RenderingContext& renderingContext;

        const Sierra::Shader& vertexShader;
        const Sierra::Shader& fragmentShader;
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
        void SetShaders(const Sierra::Shader& vertexShader, const Sierra::Shader& fragmentShader);

        [[nodiscard]] RenderTargetID CreateRenderTarget();
        bool DestroyRenderTarget(RenderTargetID ID);

        [[nodiscard]] const Sierra::Image& Render(const DrawFrame& drawFrame, const SceneRenderInfo& renderInfo);

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
        std::unique_ptr<Sierra::GraphicsPipeline> graphicsPipeline = { };

        // NOTE: Rendering is mocked up with a single triangle (a vertex of which is one of these), regardless of what the scene holds
        struct TriangleVertex
        {
            Vector3 position = { 0.0f, 0.0f, 0.0f };
            std::array<uint8, 4> color = { 0, 0, 0, 0 }; // RGBA, as read by [Sierra::VertexInput::Color]
        };
        std::unique_ptr<Sierra::Buffer> triangleVertexBuffer = nullptr;
        [[nodiscard]] std::unique_ptr<Sierra::GraphicsPipeline> CreateGraphicsPipeline(const Sierra::Shader& vertexShader, const Sierra::Shader& fragmentShader) const;

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