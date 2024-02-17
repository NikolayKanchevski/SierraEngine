//
// Created by Nikolay Kanchevski on 12/14/23.
//

#pragma once

#if !SR_PLATFORM_ANDROID
    #error "Including the GameActivityWindow.h file is only allowed in Android builds!"
#endif

#include "../../Window.h"
#include "GameActivityContext.h"

#include <game-activity/native_app_glue/android_native_app_glue.h>
#include "GameActivityTouchManager.h"

namespace Sierra
{

    class SIERRA_API GameActivityWindow final : public Window
    {
    public:
        /* --- CONSTRUCTORS --- */
        GameActivityWindow(const GameActivityContext &gameActivityContext, const WindowCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void Update() override;
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
        [[nodiscard]] PlatformAPI GetAPI() const override;

        [[nodiscard]] inline ANativeWindow* GetNativeWindow() const { return window; }

        /* --- DESTRUCTOR --- */
        ~GameActivityWindow() override;

    private:
        const GameActivityContext &gameActivityContext;

        ANativeWindow* window;
        GameActivityTouchManager touchManager;

        std::string title;
        bool minimized = false;
        bool closed = false;

        void HandleCommand(uint32 command);

    };

}
