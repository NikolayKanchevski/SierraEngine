//
// Created by Nikolay Kanchevski on 28.01.24.
//

#pragma once

#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_internal.h>

#include "../../Rendering/RenderingInstance.h"
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

    struct ImGuiRendererCreateInfo
    {
        const Device& device;
        CommandBuffer& commandBuffer;

        ResourceTable& resourceTable;
        DestructionScheduler& destructionScheduler;

        uint32 concurrentFrameCount = 1;
        ImageFormat format = ImageFormat::Undefined;
        std::span<const ImGuiFontCreateInfo> fontCreateInfos = { };
    };

    class SIERRA_API ImGuiRenderer final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit ImGuiRenderer(const ImGuiRendererCreateInfo& createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] ImFont* GetFont(const size index) const noexcept { return ImGui::GetIO().Fonts->Fonts[static_cast<int>(baseFontIndex + index)]; }

        /* --- POLLING METHODS --- */
        void Update(uint32 framebufferWidth, uint32 framebufferHeight, float32 scaling = 1.0f, const InputManager* inputManager = nullptr, const CursorManager* cursorManager = nullptr, const TouchManager* touchManager = nullptr) const;
        void Render(CommandBuffer& commandBuffer, const Framebuffer& framebuffer);

        /* --- COPY SEMANTICS --- */
        ImGuiRenderer(const ImGuiRenderer&) = delete;
        ImGuiRenderer& operator=(const ImGuiRenderer&) = delete;

        /* --- MOVE SEMANTICS --- */
        ImGuiRenderer(ImGuiRenderer&& other) = delete;
        ImGuiRenderer& operator=(ImGuiRenderer&&) = delete;

        /* --- DESTRUCTOR --- */
        ~ImGuiRenderer() noexcept;

    private:
        const Device* device;
        ResourceTable* resourceTable;
        DestructionScheduler* destructionScheduler;

        uint32 currentFrame = 0;
        uint32 concurrentFrameCount = 0;

        uint32 baseFontIndex = 0;

        SampledImageID fontAtlasID = { };
        SamplerID fontAtlasSamplerID = { };
        std::unique_ptr<Image> fontAtlas = nullptr;

        std::unique_ptr<Image> resolverImage = nullptr;
        std::vector<std::unique_ptr<Buffer>> vertexBuffers = { };
        std::vector<std::unique_ptr<Buffer>> indexBuffers = { };

        std::unique_ptr<RenderPass> renderPass = nullptr;
        std::unique_ptr<GraphicsPipeline> pipeline = nullptr;

        struct PushConstant
        {
            uint32 textureIndex = 0;
            uint32 samplerIndex = 0;
            Vector2 scale = { 0.0f, 0.0f };
        };

    };

}