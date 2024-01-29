//
// Created by Nikolay Kanchevski on 28.01.24.
//

#pragma once

#include "../../ImGuiRenderTask.h"

#include <imgui.h>
#include <imgui_internal.h>

#include "MetalDevice.h"

namespace Sierra
{

    class SIERRA_API MetalImGuiTask final : public ImGuiRenderTask
    {
    public:
        /* --- CONSTRUCTORS --- */
        MetalImGuiTask(const MetalDevice &device, const ImGuiRenderTaskCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void BeginFrame(const InputManager &inputManager, const CursorManager &cursorManager) override;

        void Resize(uint32 width, uint32 height) override;
        void Render(std::unique_ptr<CommandBuffer> &commandBuffer, const std::unique_ptr<Image> &image) override;

        /* --- DESTRUCTOR --- */
        ~MetalImGuiTask() override;

    private:
        std::unique_ptr<RenderPass> renderPass = nullptr;

    };

}
