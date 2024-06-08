//
// Created by Nikolay Kanchevski on 8.10.23.
//

#pragma once

#if !SR_PLATFORM_iOS
    #error "Including the UIKitTouchManager.h file is only allowed in iOS builds!"
#endif

#include "../../TouchManager.h"

#if defined(__OBJC__)
    #include <UIKit/UIKit.h>
#endif

namespace Sierra
{

    class SIERRA_API UIKitTouchManager final : public TouchManager
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit UIKitTouchManager();

        /* --- POLLING METHODS --- */
        void RegisterTouchPress(const Touch &touch) override;
        void RegisterTouchMove(TouchID ID, Vector2 position) override;
        void RegisterTouchRelease(TouchID ID) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::span<const Touch> GetTouches() const override { return touches; }

    private:
        std::vector<Touch> touches;

        friend class UIKitWindow;
        void Update();

    };

}
