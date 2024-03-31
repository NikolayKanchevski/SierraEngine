//
// Created by Nikolay Kanchevski on 26.09.23.
//

#pragma once

#if !SR_PLATFORM_iOS
    #error "Including the UIKitWindow.h file is only allowed in iOS builds!"
#endif

#include "../../Window.h"
#include "UIKitContext.h"
#include "UIKitTouchManager.h"
#include "UIKitSelectorBridge.h"

#if !defined(__OBJC__)
    namespace Sierra
    {
        using UIKitWindowViewController = void;
        using UIView = void;
        #define nil nullptr
    }
#else
    #include <UIKit/UIKit.h>
    @class UIKitWindowViewController;
#endif

namespace Sierra
{

    class SIERRA_API UIKitWindow final : public Window
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit UIKitWindow(const UIKitContext &uiKitContext, const WindowCreateInfo &createInfo);

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
        void SetPosition(const Vector2Int &position) override;
        void SetSize(const Vector2UInt &size) override;
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

        [[nodiscard]] const Screen& GetScreen() const override;
        [[nodiscard]] TouchManager& GetTouchManager() override;
        [[nodiscard]] PlatformAPI GetAPI() const override;

        [[nodiscard]] inline UIWindow* GetUIWindow() const { return window; }
        [[nodiscard]] inline UIView* GetUIView() const { return view; }
        [[nodiscard]] inline bool AllowsOrientationChange() const { return allowsOrientationChange; }

        /* --- DESTRUCTOR --- */
        ~UIKitWindow() override;

    private:
        const UIKitContext &uiKitContext;
        UIKitTouchManager touchManager;
        
        UIWindow* window = nil;
        UIKitWindowViewController* viewController = nil;
        UIView* view = nil;

        std::string title;
        bool minimized = false;
        bool closed = false;

        bool allowsOrientationChange = false;
        ScreenOrientation lastOrientation = ScreenOrientation::Portrait;

        UIKitSelectorBridge deviceOrientationDidChangeBridge;
        UIKitSelectorBridge applicationDidEnterBackgroundBridge;
        UIKitSelectorBridge applicationWillEnterForegroundBridge;
        UIKitSelectorBridge applicationWillTerminateBridge;
        
        /* --- EVENTS --- */
        void DeviceOrientationDidChange();
        void ApplicationDidEnterBackground();
        void ApplicationWillEnterForeground();
        void ApplicationWillTerminate();

    };

}
