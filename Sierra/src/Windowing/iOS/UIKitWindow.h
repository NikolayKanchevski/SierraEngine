//
// Created by Nikolay Kanchevski on 26.09.23.
//

#pragma once

#if !SR_PLATFORM_iOS
    #error "Including the UIKitWindow.h file is only allowed in iOS builds!"
#endif

#if defined(__OBJC__)
    #include <UIKit/UIKit.h>
#else
    namespace Sierra
    {
        #define nil nullptr
        using UIWindow = void;
        using UIViewController = void;
        using UIView = void;
    }
#endif

#include "../Window.h"

#include "UIKitTouchManager.h"
#include "../../Platform/iOS/UIKitContext.h"

namespace Sierra
{

    class SIERRA_API UIKitWindow final : public Window
    {
    public:
        /* --- CONSTRUCTORS --- */
        UIKitWindow(const UIKitContext& uiKitContext, const WindowCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        void Update() override;
        void Minimize() override;
        void Maximize() override;
        void Show() override;
        void Hide() override;
        void Focus() override;
        void Close() override;

        /* --- SETTER METHODS --- */
        void SetTitle(std::string_view title) override;
        void SetPosition(Vector2Int position) override;
        void SetSize(Vector2UInt size) override;
        void SetOpacity(float32 opacity) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetTitle() const noexcept override;
        [[nodiscard]] Vector2Int GetPosition() const noexcept override;
        [[nodiscard]] uint32 GetWidth() const noexcept override;
        [[nodiscard]] uint32 GetHeight() const noexcept override;
        [[nodiscard]] uint32 GetFramebufferWidth() const noexcept override;
        [[nodiscard]] uint32 GetFramebufferHeight() const noexcept override;
        [[nodiscard]] float32 GetOpacity() const noexcept override;

        [[nodiscard]] bool IsClosed() const noexcept override;
        [[nodiscard]] bool IsMinimized() const noexcept override;
        [[nodiscard]] bool IsMaximized() const noexcept override;
        [[nodiscard]] bool IsFocused() const noexcept override;
        [[nodiscard]] bool IsHidden() const noexcept override;

        [[nodiscard]] InputManager* GetInputManager() noexcept override;
        [[nodiscard]] CursorManager* GetCursorManager() noexcept override;
        [[nodiscard]] TouchManager* GetTouchManager() noexcept override;

        [[nodiscard]] WindowingBackendType GetBackendType() const noexcept override;

        [[nodiscard]] UIWindow* GetUIWindow() const { return window; }
        [[nodiscard]] ScreenOrientation GetAllowedOrientations() const { return allowedOrientations; }

        [[nodiscard]] UIViewController* GetUIViewController() const { return viewController; }
        [[nodiscard]] UIView* GetUIView() const { return view; }

        /* --- MOVE SEMANTICS --- */
        UIKitWindow(UIKitWindow&&) noexcept = delete;
        UIKitWindow& operator=(UIKitWindow&&) noexcept = delete;

        /* --- COPY SEMANTICS --- */
        UIKitWindow(const UIKitWindow&) = delete;
        UIKitWindow& operator=(const UIKitWindow&) = delete;

        /* --- DESTRUCTOR --- */
        ~UIKitWindow() noexcept override;

    private:
        const UIKitContext& uiKitContext;
        
        UIWindow* window = nil;
        UIViewController* viewController = nil;
        UIView* view = nil;

        UIKitTouchManager touchManager;

        std::string title;
        bool minimized = false;
        bool closed = false;
        ScreenOrientation allowedOrientations = ScreenOrientation::Unknown;

    public:
        #if defined(__OBJC__) && defined(UIKIT_WINDOW_IMPLEMENTATION)
            /* --- EVENTS --- */
            void ApplicationDidEnterBackground();
            void ApplicationWillEnterForeground();
            void SceneDidDisconnect();
        #endif

    };

}
