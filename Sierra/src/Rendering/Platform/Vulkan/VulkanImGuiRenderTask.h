//
// Created by Nikolay Kanchevski on 29.01.24.
//

#pragma once

#include "../../ImGuiRenderTask.h"

#include <imgui.h>
#include <imgui_internal.h>

#include "VulkanInstance.h"
#include "VulkanDevice.h"
#include "VulkanDescriptors.h"

namespace Sierra
{

    class SIERRA_API VulkanImGuiRenderTask final : public ImGuiRenderTask
    {
    public:
        /* --- CONSTRUCTORS --- */
        VulkanImGuiRenderTask(const VulkanInstance &instance, const VulkanDevice &device, const ImGuiRenderTaskCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void BeginFrame(const InputManager &inputManager, const CursorManager &cursorManager) override;

        void Resize(uint32 width, uint32 height) override;
        void Render(std::unique_ptr<CommandBuffer> &commandBuffer, const std::unique_ptr<Image> &image) override;

        /* --- DESTRUCTOR --- */
        ~VulkanImGuiRenderTask() override;

    private:
        std::unique_ptr<RenderPass> renderPass = nullptr;
        static inline std::unique_ptr<VulkanDescriptorPool> sharedDescriptorPool = nullptr;

    };

}