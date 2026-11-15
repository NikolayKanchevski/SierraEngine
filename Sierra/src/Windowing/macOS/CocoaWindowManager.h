//
// Created by Nikolay Kanchevski on 15.11.26.
//

#pragma once

#include "../WindowManager.h"

#include "../../Platform/macOS/CocoaContext.h"

namespace Sierra
{

    struct CocoaWindowManagerCreateInfo
    {

    };

    class SIERRA_API CocoaWindowManager final : public WindowManager
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit CocoaWindowManager(const CocoaWindowManagerCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::unique_ptr<Window> CreateWindow(const WindowCreateInfo& createInfo) const noexcept override;
        void EnumerateScreens(const ScreenEnumerationPredicate& Predicate) const noexcept override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] const Screen& GetPrimaryScreen() const noexcept override { return cocoaContext.GetScreens()[0]; } // NOTE: Cocoa has no notion of a primary screen, so the first one reported is used
        [[nodiscard]] const Screen& GetWindowScreen(const Window& window) const override;

        [[nodiscard]] WindowingBackendType GetBackendType() const noexcept override { return WindowingBackendType::Cocoa; }

        /* --- COPY SEMANTICS --- */
        CocoaWindowManager(const CocoaWindowManager&) = delete;
        CocoaWindowManager& operator=(const CocoaWindowManager&) = delete;

        /* --- MOVE SEMANTICS --- */
        CocoaWindowManager(CocoaWindowManager&&) noexcept = delete;
        CocoaWindowManager& operator=(CocoaWindowManager&&) noexcept = delete;

        /* --- DESTRUCTOR --- */
        ~CocoaWindowManager() noexcept override = default;

    private:
        CocoaContext cocoaContext;

    };

}