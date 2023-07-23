//
// Created by Nikolay Kanchevski on 22.12.22.
//

#pragma once

#include "../../Window.h"
#include "../../UI/ImGuiInstance.h"

namespace Sierra::Rendering
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
        static UniquePtr<VulkanRenderer> Create(const VulkanRendererCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void Prepare();
        virtual void Render();

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline UniquePtr<Window>& GetWindow() const { return window; }
        [[nodiscard]] UniquePtr<ImGuiInstance>& GetImGuiInstance() { return imGuiInstance; }
        [[nodiscard]] inline virtual VkDescriptorSet GetRenderedTextureDescriptorSet() const { return VK_NULL_HANDLE; };

        /* --- DESTRUCTOR --- */
        virtual void Destroy();
        DELETE_COPY(VulkanRenderer);

    protected:
        virtual void Update();
        inline virtual void DrawUI() { };

        UniquePtr<Window> &window;
        UniquePtr<Swapchain> swapchain = nullptr;
        UniquePtr<ImGuiInstance> imGuiInstance = nullptr;

        uint maxConcurrentFrames;

    private:
        bool prepared = false;
        bool hasImGuiInstance = false;

    };

}