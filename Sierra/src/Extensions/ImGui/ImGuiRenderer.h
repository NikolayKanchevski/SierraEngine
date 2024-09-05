//
// Created by Nikolay Kanchevski on 28.01.24.
//

#pragma once

#include <imgui.h>
#include "../../Rendering/Renderer.h"

#include "../../Rendering/RenderingContext.h"
#include "../../Rendering/Image.h"
#include "../../Rendering/RenderPass.h"
#include "../../Rendering/CommandBuffer.h"

#include "../../Core/InputManager.h"
#include "../../Core/CursorManager.h"

namespace Sierra
{

    struct ImGuiFontCreateInfo
    {
        float32 size = 10.0f;
        std::span<const uint8> ttfMemory = { };
    };

    struct ImGuiRenderTaskCreateInfo
    {
        const ImGuiStyle& style = { };
        std::span<const ImGuiFontCreateInfo> fontCreateInfos = { };

        uint32 concurrentFrameCount = 0;
        const RenderingContext& renderingContext;
        CommandBuffer& commandBuffer;

        uint32 scaling = 1;
        ImageSampling sampling = ImageSampling::x1;
        const Image& templateOutputImage;

        ResourceIndex fontAtlasIndex = 0;
        ResourceIndex fontSamplerIndex = 0;
        ResourceTable& resourceTable;
    };

    class SIERRA_API ImGuiRenderer final : public Renderer
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit ImGuiRenderer(const ImGuiRenderTaskCreateInfo& createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] ImFont* GetFont(const size index) const { return ImGui::GetIO().Fonts->Fonts[static_cast<int>(baseFontIndex + index)]; }
        [[nodiscard]] ImGuiStyle& GetStyle() { return style; }

        /* --- POLLING METHODS --- */
        void Update(const InputManager* inputManager = nullptr, const CursorManager* cursorManager = nullptr, const TouchManager* touchManager = nullptr);
        void Resize(uint32 width, uint32 height) override;
        void Render(CommandBuffer& commandBuffer, const Image& outputImage) override;

        /* --- DESTRUCTOR --- */
        ~ImGuiRenderer() override;

    private:
        const RenderingContext& renderingContext;

        const uint32 concurrentFrameCount = 0;
        uint32 currentFrame = 0;

        uint32 scaling = 1;
        Vector2 viewportSize = { 0.0f, 0.0f };

        std::unique_ptr<Image> resolverImage = nullptr;
        std::vector<std::unique_ptr<Buffer>> vertexBuffers = { };
        std::vector<std::unique_ptr<Buffer>> indexBuffers = { };

        uint32 baseFontIndex = 0;
        std::unique_ptr<Image> fontAtlas = nullptr;

        std::unique_ptr<RenderPass> renderPass;
        std::unique_ptr<GraphicsPipeline> pipeline;
        ImGuiStyle style = { };

        struct PushConstant
        {
            ResourceIndex textureIndex = 0;
            ResourceIndex samplerIndex = 0;
            Vector2 scale = { 0.0f, 0.0f };
        };

        inline static struct {
            ImGuiContext* context = nullptr;
            uint32 contextCount = 0;

            std::unique_ptr<Shader> vertexShader = nullptr;
            std::unique_ptr<Shader> fragmentShader = nullptr;

            ResourceIndex fontSamplerIndex = 0;
            std::unique_ptr<Sampler> fontSampler = nullptr;
        } sharedResources = { nullptr, 0, nullptr, nullptr, 0, nullptr };

    };

}