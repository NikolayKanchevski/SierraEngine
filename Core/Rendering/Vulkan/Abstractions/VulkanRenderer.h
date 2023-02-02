//
// Created by Nikolay Kanchevski on 22.12.22.
//

#pragma once

#include "../../Window.h"
#include "../../UI/ImGuiInstance.h"
#include "Swapchain.h"

using Rendering::UI::ImGuiInstance;

namespace Sierra::Core::Rendering::Vulkan::Abstractions
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
        virtual void Render();

        /* --- SETTER METHODS --- */
        inline virtual void SetShadingType(const ShadingType newShadingType) { shadingType = newShadingType; };
        inline virtual void SetSampling(const Sampling newSampling) { sampling = newSampling; };

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline UniquePtr<Window>& GetWindow() const { return window; }
        [[nodiscard]] float GetTotalDrawTime() const { return totalDrawTime; }
        [[nodiscard]] UniquePtr<ImGuiInstance>& GetImGuiInstance() { return imGuiInstance; }
        [[nodiscard]] uint64 GetTotalVerticesDrawn() const { return totalVerticesDrawn; }

        /* --- DESTRUCTOR --- */
        virtual void Destroy();
        VulkanRenderer(const VulkanRenderer &) = delete;
        VulkanRenderer &operator=(const VulkanRenderer &) = delete;

    protected:
        UniquePtr<Window> &window;
        UniquePtr<Swapchain> swapchain;
        UniquePtr<ImGuiInstance> imGuiInstance = nullptr;

        Sampling sampling = Sampling::MSAAx1;
        ShadingType shadingType = ShadingType::FILL;
        uint maxConcurrentFrames;

        float totalDrawTime = 0.0f;
        uint64 totalVerticesDrawn = 0;

    private:
        bool prepared = false;
        bool hasImGuiInstance = false;

};

}