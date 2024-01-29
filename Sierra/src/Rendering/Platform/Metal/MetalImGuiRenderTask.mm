//
// Created by Nikolay Kanchevski on 28.01.24.
//

#include "MetalImGuiRenderTask.h"

#include <backends/imgui_impl_metal.h>

#include "MetalRenderPass.h"
#include "MetalCommandBuffer.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MetalImGuiTask::MetalImGuiTask(const MetalDevice &device, const ImGuiRenderTaskCreateInfo &createInfo)
        : ImGuiRenderTask(createInfo)
    {
        SR_ERROR_IF(createInfo.templateImage->GetAPI() != GraphicsAPI::Metal, "[Metal]: Template image passed to Metal-implemented ImGui render task has a graphics API, which differs from [GraphicsAPI::Metal]!");

        // Create render pass
        renderPass = std::make_unique<MetalRenderPass>(device, RenderPassCreateInfo {
            .name = "Render pass of ImGui task [" + std::to_string(GetActiveTaskCount() - 1) + "]",
            .attachments = { { .templateImage = createInfo.templateImage } },
            .subpassDescriptions = {
                { .renderTargets = { 0 } }
            }
        });

        // Initialize ImGui Metal backend
        if (GetActiveTaskCount() == 1) SR_ERROR_IF(!ImGui_ImplMetal_Init((__bridge id<MTLDevice>) device.GetMetalDevice()), "[Metal]: Could not create Metal ImGui backend!");
    }

    /* --- POLLING METHODS --- */

    void MetalImGuiTask::BeginFrame(const InputManager &inputManager, const CursorManager &cursorManager)
    {
        ImGuiRenderTask::BeginFrame(inputManager, cursorManager);

        ImGui_ImplMetal_NewFrame((__bridge MTLRenderPassDescriptor*) static_cast<MetalRenderPass&>(*renderPass).GetSubpass(0));
        ImGui::NewFrame();
    }

    void MetalImGuiTask::Resize(const uint32 width, const uint32 height)
    {
        ImGuiRenderTask::Resize(width, height);
        renderPass->Resize(width, height);
    }

    void MetalImGuiTask::Render(std::unique_ptr<CommandBuffer> &commandBuffer, const std::unique_ptr<Image> &image)
    {
        SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot execute a Metal-implemented ImGui render task using command buffer [{0}], whose graphics API differs from [GraphicsAPI::Metal]!!", commandBuffer->GetName());
        const MetalCommandBuffer &metalCommandBuffer = static_cast<MetalCommandBuffer&>(*commandBuffer);

        SR_ERROR_IF(image->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot render a Metal-implemented ImGui render task using image [{0}], whose graphics API differs from [GraphicsAPI::Metal]!", image->GetName());
        if (ImGui::GetDrawData() == nullptr) return;

        // Draw ImGui image
        commandBuffer->BeginRenderPass(renderPass, { { .image = image } });
        ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), (__bridge id<MTLCommandBuffer>) metalCommandBuffer.GetMetalCommandBuffer(), (__bridge id<MTLRenderCommandEncoder>) metalCommandBuffer.GetCurrentRenderCommandEncoder());
        commandBuffer->EndRenderPass(renderPass);
    }

    /* --- DESTRUCTOR --- */

    MetalImGuiTask::~MetalImGuiTask()
    {
        if (GetActiveTaskCount() == 1)
        {
            ImGui_ImplMetal_Shutdown();
            ImGui::Shutdown();
        }
    }

}