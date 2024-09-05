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
#include "../../Window.h"

#include "UIKitContext.h"
#include "UIKitTouchManager.h"

namespace Sierra
{

    class SIERRA_API UIKitWindow final : public Window
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit UIKitWindow(UIKitContext& uiKitContext, const WindowCreateInfo& createInfo);

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
        void SetPosition(const Vector2Int& position) override;
        void SetSize(const Vector2UInt& size) override;
        void SetOpacity(float32 opacity) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetTitle() const override;
        [[nodiscard]] Vector2Int GetPosition() const override;
        [[nodiscard]] uint32 GetWidth() const override;
        [[nodiscard]] uint32 GetHeight() const override;
        [[nodiscard]] uint32 GetFramebufferWidth() const override;
        [[nodiscard]] uint32 GetFramebufferHeight() const override;
        [[nodiscard]] float32 GetOpacity() const override;

        [[nodiscard]] bool IsClosed() const override;
        [[nodiscard]] bool IsMinimized() const override;
        [[nodiscard]] bool IsMaximized() const override;
        [[nodiscard]] bool IsFocused() const override;
        [[nodiscard]] bool IsHidden() const override;

        [[nodiscard]] Screen& GetScreen() const override;
        [[nodiscard]] InputManager* GetInputManager() override;
        [[nodiscard]] CursorManager* GetCursorManager() override;
        [[nodiscard]] TouchManager* GetTouchManager() override;
        [[nodiscard]] PlatformAPI GetAPI() const override;

        [[nodiscard]] UIWindow* GetUIWindow() const { return window; }
        [[nodiscard]] ScreenOrientation GetAllowedOrientations() const { return allowedOrientations; }

        [[nodiscard]] UIViewController* GetUIViewController() const { return viewController; }
        [[nodiscard]] UIView* GetUIView() const { return view; }

        /* --- DESTRUCTOR --- */
        ~UIKitWindow() override;

    private:
        UIKitContext& uiKitContext;
        
        UIWindow* window = nil;
        UIViewController* viewController = nil;
        UIView* view = nil;

        UIKitTouchManager touchManager;

        std::string title;
        bool minimized = false;
        bool closed = false;
        ScreenOrientation allowedOrientations = ScreenOrientation::Unknown;

        #if defined(__OBJC__) && defined(UIKIT_WINDOW_IMPLEMENTATION)
            public:
                /* --- EVENTS --- */
                void ApplicationDidEnterBackground();
                void ApplicationWillEnterForeground();
                void SceneDidDisconnect();
        #endif


    };

}
