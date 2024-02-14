//
// Created by Nikolay Kanchevski on 6.12.23.
//

#pragma once

#include "RenderingResource.h"
#include "../Core/Window.h"

#include "RenderPass.h"
#include "CommandBuffer.h"

namespace Sierra
{

    #pragma region Events
        class SIERRA_API SwapchainEvent : public Event { };

        class SIERRA_API SwapchainResizeEvent final : public SwapchainEvent
        {
        public:
            /* --- CONSTRUCTORS --- */
            inline explicit SwapchainResizeEvent(const Vector2UInt size) : size(size) { }

            /* --- GETTER METHODS --- */
            [[nodiscard]] inline Vector2UInt GetSize() const { return size; }

        private:
            Vector2UInt size;

        };
    #pragma endregion

    enum class SwapchainPresentationMode : bool
    {
        Immediate,
        VSync
    };

    enum class SwapchainImageMemoryType : uint8
    {
        UNorm8,
        SRGB8,
        UNorm16
    };

    struct SwapchainCreateInfo
    {
        const std::string &name = "Swapchain";
        std::unique_ptr<Window> &window;
        SwapchainPresentationMode preferredPresentationMode = SwapchainPresentationMode::VSync;
        SwapchainImageMemoryType preferredImageMemoryType = SwapchainImageMemoryType::UNorm8;
    };

    class SIERRA_API Swapchain : public virtual RenderingResource
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        template<typename T>
        using SwapchainEventCallback = std::function<bool(const T&)>;

        /* --- POLLING METHODS --- */
        virtual void AcquireNextImage() = 0;
        virtual void Present(std::unique_ptr<CommandBuffer> &commandBuffer) = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual uint32 GetCurrentFrame() const = 0;
        [[nodiscard]] virtual uint32 GetConcurrentFrameCount() const = 0;

        [[nodiscard]] virtual const std::unique_ptr<Image>& GetCurrentImage() const = 0;
        [[nodiscard]] virtual const std::unique_ptr<Image>& GetImage(uint32 frameIndex) const = 0;

        [[nodiscard]] inline uint32 GetWidth() const { return GetImage(0)->GetWidth(); };
        [[nodiscard]] inline uint32 GetHeight() const { return GetImage(0)->GetHeight(); };

        /* --- EVENTS --- */
        template<typename T> void OnEvent(SwapchainEventCallback<T> Callback) { static_assert(std::is_base_of_v<SwapchainEvent, T> && !std::is_same_v<SwapchainEvent, T>, "Template function accepts derived swapchain events only!"); }

        /* --- OPERATORS --- */
        Swapchain(const Swapchain&) = delete;
        Swapchain& operator=(const Swapchain&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~Swapchain() = default;

    protected:
        explicit Swapchain(const SwapchainCreateInfo &createInfo);
        [[nodiscard]] inline EventDispatcher<SwapchainResizeEvent>& GetSwapchainResizeDispatcher() { return swapchainResizeDispatcher; };

    private:
        EventDispatcher<SwapchainResizeEvent> swapchainResizeDispatcher;

    };

    template<> inline void Swapchain::OnEvent<SwapchainResizeEvent>(SwapchainEventCallback<SwapchainResizeEvent> Callback) { swapchainResizeDispatcher.Subscribe(Callback); }

}
