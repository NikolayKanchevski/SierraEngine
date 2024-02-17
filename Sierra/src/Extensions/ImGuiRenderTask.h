//
// Created by Nikolay Kanchevski on 28.01.24.
//

#pragma once

#define IM_VEC2_CLASS_EXTRA                                                                       \
		ImVec2(const Vector2 &other) { x = other.x; y = other.y; }                                \
		operator Vector2() const { return Vector2(x, y); }

#define IM_VEC4_CLASS_EXTRA                                                                       \
		ImVec4(const Vector4 &other) { x = other.x; y = other.y; z = other.z; w = other.w; }      \
		operator Vector4() const { return Vector4(x, y, z, w); }

#define ImDrawIdx uint32_t
#include <imgui.h>
#include <imgui_internal.h>

#include "../Rendering/CommandTask.h"

#include "../Rendering/RenderingContext.h"
#include "../Rendering/Image.h"
#include "../Rendering/RenderPass.h"
#include "../Rendering/CommandBuffer.h"

#include "../Core/InputManager.h"
#include "../Core/CursorManager.h"

namespace Sierra
{

    struct ImGuiRenderTaskCreateInfo
    {
        const std::string &name = "ImGui Render Task";
        const ImGuiStyle &style = { };

        const std::unique_ptr<Image> &templateImage;
        float32 scaling = 1.0f;
    };

    class SIERRA_API ImGuiRenderTask : public CommandTask
    {
    public:
        /* --- CONSTRUCTORS --- */
        ImGuiRenderTask(const RenderingContext &renderingContext, const ImGuiRenderTaskCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        static void CreateResources(const RenderingContext &renderingContext, std::unique_ptr<CommandBuffer> &commandBuffer);
        static void DestroyResources();

        void Update(const std::optional<std::reference_wrapper<const InputManager>> &inputManager = std::nullopt, const std::optional<std::reference_wrapper<const CursorManager>> &cursorManager = std::nullopt, const std::optional<std::reference_wrapper<const TouchManager>> &touchManager = std::nullopt);
        void Resize(uint32 width, uint32 height);
        void Render(std::unique_ptr<CommandBuffer> &commandBuffer, const std::unique_ptr<Image> &image);

        /* --- DESTRUCTOR --- */
        ~ImGuiRenderTask() override = default;

    private:
        const RenderingContext &renderingContext;
        float32 scaling = 0.0f;
        Vector2 framebufferSize = { 0.0f, 0.0f };

        struct PushConstant
        {
            Vector2 translation = { 0.0f, 0.0f };
            Vector2 scale = { 0.0f, 0.0f };
        };

        std::unique_ptr<Buffer> vertexBuffer = nullptr;
        std::unique_ptr<Buffer> indexBuffer = nullptr;

        std::unique_ptr<RenderPass> renderPass;
        std::unique_ptr<GraphicsPipeline> pipeline;
        ImGuiStyle style = { };

        static inline struct {
            ImGuiContext* context = nullptr;
            std::unique_ptr<Shader> vertexShader = nullptr;
            std::unique_ptr<Shader> fragmentShader = nullptr;
            std::unique_ptr<PipelineLayout> pipelineLayout = nullptr;
            std::unique_ptr<Sampler> fontSampler = nullptr;
            std::unique_ptr<Image> defaultFontAtlas = nullptr;
        } sharedResources = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

    };

}
