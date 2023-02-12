//
// Created by Nikolay Kanchevski on 22.12.22.
//

#pragma once

#include "../../Window.h"
#include "../../UI/ImGuiInstance.h"
#include "../Abstractions/Abstractions.h"

using Rendering::UI::ImGuiInstance;

namespace Sierra::Core::Rendering::Vulkan::Renderers
{

    struct VulkanRendererCreateInfo
    {
        UniquePtr<Window> &window;
        bool createImGuiInstance = false;
        bool createImGuizmoLayer = false;
    };

    class VulkanRenderer
    {
    public:
        /* --- CONSTRUCTORS --- */
        VulkanRenderer(const VulkanRendererCreateInfo &createInfo);
        static UniquePtr<VulkanRenderer> Create(VulkanRendererCreateInfo createInfo);

        /* --- POLLING METHODS --- */
        void Prepare();
        virtual void Update();
        inline virtual void DrawUI() { };
        virtual void Render();

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline virtual VkDescriptorSet GetRenderedTextureDescriptorSet() const { return VK_NULL_HANDLE; };
        [[nodiscard]] inline UniquePtr<Window>& GetWindow() const { return window; }
        [[nodiscard]] float GetTotalDrawTime() const { return totalDrawTime; }
        [[nodiscard]] UniquePtr<ImGuiInstance>& GetImGuiInstance() { return imGuiInstance; }
        [[nodiscard]] uint64 GetTotalVerticesDrawn() const { return totalVerticesDrawn; }

        /* --- DESTRUCTOR --- */
        virtual void Destroy();
        DELETE_COPY(VulkanRenderer);

    protected:
        UniquePtr<Window> &window;
        UniquePtr<Swapchain> swapchain;
        UniquePtr<ImGuiInstance> imGuiInstance = nullptr;

        uint maxConcurrentFrames;

        float totalDrawTime = 0.0f;
        uint64 totalVerticesDrawn = 0;

    private:
        bool prepared = false;
        bool hasImGuiInstance = false;

};

}