//
// Created by Nikolay Kanchevski on 28.01.24.
//

#pragma once

#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_internal.h>

#include "../../Core/ApplicationFrame.h"

namespace SierraEngine
{

    struct ImGuiFontCreateInfo
    {
        float32 size = 21.0f;
        std::span<const uint8> ttfBuffer = { };
    };

    struct ImGuiRendererCreateInfo
    {
        const Sierra::Device& device;
        Sierra::ResourceTable& resourceTable;

        uint32 concurrentFrameCount = 1;
        Sierra::ImageFormat format = Sierra::ImageFormat::Undefined;

        std::span<const ImGuiFontCreateInfo> fontCreateInfos = { };
    };

    class ImGuiRenderer final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit ImGuiRenderer(const ImGuiRendererCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        void Update(const ApplicationFrame& applicationFrame, Vector2UInt framebufferSize, float32 scaling = 1.0f) const;
        void Render(const DrawFrame& drawFrame, const Sierra::Framebuffer& framebuffer);

        /* --- GETTER METHODS --- */
        [[nodiscard]] ImFont* GetFont(const size index) const noexcept { return ImGui::GetIO().Fonts->Fonts[static_cast<int>(index)]; }

        /* --- COPY SEMANTICS --- */
        ImGuiRenderer(const ImGuiRenderer&) = delete;
        ImGuiRenderer& operator=(const ImGuiRenderer&) = delete;

        /* --- MOVE SEMANTICS --- */
        ImGuiRenderer(ImGuiRenderer&&) = delete;
        ImGuiRenderer& operator=(ImGuiRenderer&&) = delete;

        /* --- DESTRUCTOR --- */
        ~ImGuiRenderer() noexcept;

    private:
        struct Texture
        {
            std::unique_ptr<Sierra::Image> image = nullptr;
            Sierra::SampledImageID ID = { };
        };

        struct PushConstant
        {
            uint32 textureIndex = 0;
            uint32 samplerIndex = 0;
            Vector2 scale = { 0.0f, 0.0f };
        };

        ImGuiContext* context = nullptr;
        Sierra::ResourceTable& resourceTable;

        uint32 concurrentFrameCount = 0;
        uint32 currentFrame = 0;

        std::unique_ptr<Sierra::Shader> vertexShader = nullptr;
        std::unique_ptr<Sierra::Shader> fragmentShader = nullptr;
        std::unique_ptr<Sierra::RenderPass> renderPass = nullptr;
        std::unique_ptr<Sierra::GraphicsPipeline> pipeline = nullptr;

        std::vector<std::unique_ptr<Sierra::Buffer>> vertexBuffers = { };
        std::vector<std::unique_ptr<Sierra::Buffer>> indexBuffers = { };

        std::unique_ptr<Sierra::Sampler> sampler = nullptr;
        Sierra::SamplerID samplerID = { };
        std::vector<std::unique_ptr<Texture>> textures = { };

        void UpdateTexture(const DrawFrame& drawFrame, ImTextureData& textureData);
        void DestroyTexture(ImTextureData& textureData, Sierra::DestructionScheduler* destructionScheduler);

    };

}
