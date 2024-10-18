//
// Created by Nikolay Kanchevski on 12/14/23.
//

#pragma once

#if !SR_PLATFORM_ANDROID
    #error "Including the GameActivityWindow.h file is only allowed in Android builds!"
#endif

#include "../Window.h"
#include "GameKitTouchManager.h"
#include "../../Platform/Android/GameKitContext.h"

namespace Sierra
{

    class SIERRA_API GameKitWindow final : public Window
    {
    public:
        /* --- CONSTRUCTORS --- */
        GameKitWindow(const GameKitContext& gameKitContext, const WindowCreateInfo& createInfo);

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

        [[nodiscard]] ANativeWindow* GetNativeWindow() const { return window; }

        /* --- MOVE SEMANTICS --- */
        GameKitWindow(GameKitWindow&&) noexcept = delete;
        GameKitWindow& operator=(GameKitWindow&&) noexcept = delete;

        /* --- COPY SEMANTICS --- */
        GameKitWindow(const GameKitWindow&) = delete;
        GameKitWindow& operator=(const GameKitWindow&) = delete;

        /* --- DESTRUCTOR --- */
        ~GameKitWindow() noexcept override;

    private:
        const GameKitContext& gameKitContext;

        ANativeWindow* window;
        GameKitTouchManager touchManager;

        std::string title;
        bool minimized = false;
        bool closed = false;

        static void OnCommand(uint32 command, void* userData);

    };

}
