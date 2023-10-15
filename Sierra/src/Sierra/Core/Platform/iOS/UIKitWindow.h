//
// Created by Nikolay Kanchevski on 26.09.23.
//

#pragma once

#if !SR_PLATFORM_iOS
    #error "Including the UIKitWindow.h file is only allowed in iOS builds!"
#endif

#include "../../Window.h"
#import "iOSInstance.h"
#import "UIKitTouchManager.h"

#if !defined(__OBJC__)
    namespace Sierra
    {
        typedef void UIKitWindowImplementation;
        typedef void UIKitWindowViewController;
    }
#else
    #include <UIKit/UIKit.h>
    @class UIKitWindowImplementation;

    @interface UIKitWindowViewController : UIViewController

        /* --- POLLING METHODS --- */
        - (void) applicationWillTerminate;

    @end
#endif

namespace Sierra
{

    class SIERRA_API UIKitWindow final : public Window
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit UIKitWindow(const WindowCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void OnUpdate() override;
        void Minimize() override;
        void Maximize() override;
        void Show() override;
        void Hide() override;
        void Focus() override;
        void Close() override;

        /* --- SETTER METHODS --- */
        void SetTitle(const String &title) override;
        void SetPosition(const Vector2Int &position) override;
        void SetSize(const Vector2UInt &size) override;
        void SetOpacity(float32 opacity) override;

        /* --- GETTER METHODS --- */
        String GetTitle() const override;
        Vector2Int GetPosition() const override;
        Vector2UInt GetSize() const override;
        Vector2UInt GetFramebufferSize() const override;
        float32 GetOpacity() const override;
        bool IsClosed() const override;
        bool IsMinimized() const override;
        bool IsMaximized() const override;
        bool IsFocused() const override;
        bool IsHidden() const override;
        TouchManager& GetTouchManager() override;
        WindowAPI GetAPI() const override;

        /* --- EVENTS --- */
        #if defined(__OBJC__)
            void ApplicationDidEnterBackground();
            void ApplicationWillEnterForeground();
            void ApplicationWillTerminate();
        #endif

        /* --- DESTRUCTOR --- */
        ~UIKitWindow();

    private:
        iOSInstance &iOSInstance;
        UIKitTouchManager touchManager;
        
        String title;
        bool minimized = false;
        bool closed = false;

        UIKitWindowViewController* viewController = nullptr;
        UIKitWindowImplementation* window = nullptr;

    };

}
