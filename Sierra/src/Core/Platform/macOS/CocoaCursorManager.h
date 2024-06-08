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
        #define nil nullptr
        using NSWindow = void;
    }
#endif
#include "../../CursorManager.h"

namespace Sierra
{

    class SIERRA_API CocoaCursorManager final : public CursorManager
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit CocoaCursorManager(const NSWindow* window);

        /* --- POLLING METHODS --- */
        void RegisterCursorMove(Vector2 position) override;

        /* --- SETTER METHODS --- */
        void SetCursorVisibility(bool visible) override;
        void SetCursorPosition(Vector2 position) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] bool IsCursorVisible() const override;
        [[nodiscard]] Vector2 GetCursorPosition() const override;
        [[nodiscard]] Vector2 GetCursorDelta() const override;


    private:
        const NSWindow* window;

        Vector2 cursorPosition = { 0, 0 };
        Vector2 lastCursorPosition = { 0, 0 };

        bool cursorShown = true;
        bool justHidCursor = false;

        friend class CocoaWindow;
        void Update();
        void PostUpdate();
    };

}
