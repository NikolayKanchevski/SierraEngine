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
        std::unique_ptr<Window> &window;
        bool createImGuiInstance = false;
        bool createImGuizmoLayer = false;
    };

    class VulkanRenderer
    {
    public:
        /* --- CONSTRUCTORS --- */
        VulkanRenderer(const VulkanRendererCreateInfo &createInfo);
        static std::unique_ptr<VulkanRenderer> Create(VulkanRendererCreateInfo createInfo);

        /* --- POLLING METHODS --- */
        void Prepare();
        virtual void Update();
        virtual void Render();

        /* --- SETTER METHODS --- */
        inline virtual void SetShadingType(const ShadingType newShadingType) { shadingType = newShadingType; };
        inline virtual void SetSampling(const Sampling newSampling) { sampling = newSampling; };

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline std::unique_ptr<Window>& GetWindow() const { return window; }
        [[nodiscard]] float GetTotalDrawTime() const { return totalDrawTime; }
        [[nodiscard]] std::unique_ptr<ImGuiInstance>& GetImGuiInstance() { return imGuiInstance; }
        [[nodiscard]] uint64_t GetTotalVerticesDrawn() const { return totalVerticesDrawn; }

        /* --- DESTRUCTOR --- */
        virtual void Destroy();
        VulkanRenderer(const VulkanRenderer &) = delete;
        VulkanRenderer &operator=(const VulkanRenderer &) = delete;

    protected:
        std::unique_ptr<Window> &window;
        std::unique_ptr<Swapchain> swapchain;
        std::unique_ptr<ImGuiInstance> imGuiInstance = nullptr;

        Sampling sampling = MSAAx1;
        ShadingType shadingType = SHADE_FILL;
        uint32_t maxConcurrentFrames;

        float totalDrawTime = 0.0f;
        uint64_t totalVerticesDrawn = 0;

    private:
        bool prepared = false;
        bool hasImGuiInstance = false;

};

}