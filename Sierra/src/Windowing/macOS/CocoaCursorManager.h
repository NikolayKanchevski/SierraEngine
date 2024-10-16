//
// Created by Nikolay Kanchevski on 3.10.23.
//

#pragma once

#if !SR_PLATFORM_macOS
    #error "Including the CocoaCursorManager.h file is only allowed in macOS builds!"
#endif

#if defined(__OBJC__)
    #include <Cocoa/Cocoa.h>
#else
    namespace Sierra
    {
        using NSWindow = void;
    }
#endif

#include "../CursorManager.h"

namespace Sierra
{

    class SIERRA_API CocoaCursorManager final : public CursorManager
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit CocoaCursorManager(const NSWindow* window);

        /* --- POLLING METHODS --- */
        void Update();
        void RegisterCursorMove(Vector2 position) override;

        /* --- SETTER METHODS --- */
        void SetCursorVisibility(bool visible) override;
        void SetCursorPosition(Vector2 position) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] bool IsCursorVisible() const noexcept override;
        [[nodiscard]] Vector2 GetCursorPosition() const noexcept override;
        [[nodiscard]] Vector2 GetCursorDelta() const noexcept override;
        [[nodiscard]] WindowingBackendType GetBackendType() const noexcept override;

        /* --- COPY SEMANTICS --- */
        CocoaCursorManager(const CocoaCursorManager&) = delete;
        CocoaCursorManager& operator=(const CocoaCursorManager&) = delete;

        /* --- MOVE SEMANTICS --- */
        CocoaCursorManager(CocoaCursorManager&&) = delete;
        CocoaCursorManager& operator=(CocoaCursorManager&&) = delete;

        /* --- DESTRUCTOR --- */
        ~CocoaCursorManager() noexcept override = default;

    private:
        const NSWindow* window;

        Vector2 cursorPosition = { 0, 0 };
        Vector2 lastCursorPosition = { 0, 0 };

        bool cursorShown = true;
        bool justHidCursor = false;

    };

}
