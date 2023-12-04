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
        typedef void UIKitWindowImplementation;
        typedef void UIKitWindowViewController;
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
        void OnUpdate() override;
        void Minimize() override;
        void Maximize() override;
        void Show() override;
        void Hide() override;
        void Focus() override;
        void Close() override;

        /* --- SETTER METHODS --- */
        void SetTitle(const std::string &title) override;
        void SetPosition(const Vector2Int &position) override;
        void SetSize(const Vector2UInt &size) override;
        void SetOpacity(float32 opacity) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] const std::string& GetTitle() const override;
        [[nodiscard]] Vector2Int GetPosition() const override;
        [[nodiscard]] Vector2UInt GetSize() const override;
        [[nodiscard]] Vector2UInt GetFramebufferSize() const override;
        [[nodiscard]] float32 GetOpacity() const override;
        [[nodiscard]] bool IsClosed() const override;
        [[nodiscard]] bool IsMinimized() const override;
        [[nodiscard]] bool IsMaximized() const override;
        [[nodiscard]] bool IsFocused() const override;
        [[nodiscard]] bool IsHidden() const override;

        [[nodiscard]] const Screen& GetScreen() const override;
        [[nodiscard]] TouchManager& GetTouchManager() override;
        [[nodiscard]] WindowAPI GetAPI() const override;

        /* --- DESTRUCTOR --- */
        ~UIKitWindow();

    private:
        const UIKitContext &uiKitContext;
        UIKitTouchManager touchManager;
        
        UIWindow* window = nullptr;
        UIKitWindowViewController* viewController = nullptr;
        
        std::string title;
        bool minimized = false;
        bool closed = false;

        UIKitSelectorBridge applicationDidEnterBackgroundBridge;
        UIKitSelectorBridge applicationWillEnterForegroundBridge;
        UIKitSelectorBridge applicationWillTerminateBridge;
        
        /* --- EVENTS --- */
        void ApplicationDidEnterBackground();
        void ApplicationWillEnterForeground();
        void ApplicationWillTerminate();

    };

}
