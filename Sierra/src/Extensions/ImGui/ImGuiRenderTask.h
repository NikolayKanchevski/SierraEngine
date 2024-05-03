//
// Created by Nikolay Kanchevski on 28.01.24.
//

#pragma once

#include <imgui.h>

#include "../../Rendering/RenderingContext.h"
#include "../../Rendering/Image.h"
#include "../../Rendering/RenderPass.h"
#include "../../Rendering/CommandBuffer.h"

#include "../../Core/InputManager.h"
#include "../../Core/CursorManager.h"

namespace Sierra
{

    struct ImGuiRenderTaskFontCreateInfo
    {
        float32 size = 12.0f;
        std::span<const uint8> ttfMemory = { };
    };

    struct ImGuiRenderTaskCreateInfo
    {
        std::string_view name = "ImGui Render Task";

        uint32 concurrentFrameCount = 1;
        const RenderingContext &renderingContext;
        std::unique_ptr<CommandBuffer> &commandBuffer;

        uint32 scaling = 1;
        ImageSampling sampling = ImageSampling::x1;
        const std::unique_ptr<Image> &templateOutputImage;

        const ImGuiStyle &style = { };
        ResourceTable::ResourceIndex fontAtlasIndex = 0;
        const std::span<const ImGuiRenderTaskFontCreateInfo> &fontCreateInfos = { };

        ResourceTable::ResourceIndex fontSamplerIndex = 0;
        std::unique_ptr<ResourceTable> &resourceTable;
    };

    class SIERRA_API ImGuiRenderTask
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit ImGuiRenderTask(const ImGuiRenderTaskCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline ImFont* GetFont(const uint32 index) const { return ImGui::GetIO().Fonts->Fonts[static_cast<int>(baseFontIndex + index)]; }
        [[nodiscard]] inline ImGuiStyle& GetStyle() { return style; }

        /* --- POLLING METHODS --- */
        void Update(const std::optional<std::reference_wrapper<const InputManager>> &inputManager = std::nullopt, const std::optional<std::reference_wrapper<const CursorManager>> &cursorManager = std::nullopt, const std::optional<std::reference_wrapper<const TouchManager>> &touchManager = std::nullopt);
        void Resize(uint32 width, uint32 height, uint32 scaling);
        void Render(std::unique_ptr<CommandBuffer> &commandBuffer, const std::unique_ptr<Image> &outputImage);

        /* --- DESTRUCTOR --- */
        ~ImGuiRenderTask();

    private:
        const RenderingContext &renderingContext;

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
            ResourceTable::ResourceIndex fontAtlasIndex = 0;
            ResourceTable::ResourceIndex fontSamplerIndex = 0;
            Vector2 scale = { 0.0f, 0.0f };
        };

        static inline struct {
            ImGuiContext* context = nullptr;
            uint32 contextCount = 0;

            std::unique_ptr<Shader> vertexShader = nullptr;
            std::unique_ptr<Shader> fragmentShader = nullptr;

            ResourceTable::ResourceIndex fontSamplerIndex = 0;
            std::unique_ptr<Sampler> fontSampler = nullptr;
        } sharedResources = { nullptr, 0, nullptr, nullptr, 0, nullptr };

    };

}