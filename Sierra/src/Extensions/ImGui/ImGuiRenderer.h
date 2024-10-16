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

#include "../../Windowing/InputManager.h"
#include "../../Windowing/CursorManager.h"
#include "../../Windowing/TouchManager.h"

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
        const Device& device;
        CommandBuffer& commandBuffer;

        uint32 scaling = 1;
        ImageSampling sampling = ImageSampling::x1;
        const Image& templateOutputImage;

        uint32 fontAtlasIndex = 0;
        uint32 fontSamplerIndex = 0;
        ResourceTable& resourceTable;
    };

    class SIERRA_API ImGuiRenderer final : public Renderer
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit ImGuiRenderer(const ImGuiRenderTaskCreateInfo& createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] ImFont* GetFont(const size index) const noexcept { return ImGui::GetIO().Fonts->Fonts[static_cast<int>(baseFontIndex + index)]; }
        [[nodiscard]] ImGuiStyle& GetStyle() noexcept { return style; }

        /* --- POLLING METHODS --- */
        void Update(const InputManager* inputManager = nullptr, const CursorManager* cursorManager = nullptr, const TouchManager* touchManager = nullptr);
        void Resize(uint32 width, uint32 height) override;
        void Render(CommandBuffer& commandBuffer, const Image& outputImage) override;

        /* --- COPY SEMANTICS --- */
        ImGuiRenderer(const ImGuiRenderer&) = delete;
        ImGuiRenderer& operator=(const ImGuiRenderer&) = delete;

        /* --- MOVE SEMANTICS --- */
        ImGuiRenderer(ImGuiRenderer&&) = delete;
        ImGuiRenderer& operator=(ImGuiRenderer&&) = delete;

        /* --- DESTRUCTOR --- */
        ~ImGuiRenderer() noexcept override;

    private:
        const Device& device;
        const uint32 concurrentFrameCount = 0;

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

        uint32 currentFrame = 0;
        struct PushConstant
        {
            uint32 textureIndex = 0;
            uint32 samplerIndex = 0;
            Vector2 scale = { 0.0f, 0.0f };
        };

    };

}