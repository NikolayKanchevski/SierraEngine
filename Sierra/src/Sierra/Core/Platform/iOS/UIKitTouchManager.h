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
        explicit UIKitTouchManager(const TouchManagerCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] uint32 GetTouchCount() override;
        [[nodiscard]] std::optional<Touch> GetTouch(uint32 touchIndex) override;

        /* --- EVENTS --- */
        #if defined(__OBJC__)
            // TODO(iOS): Hide these away
            void TouchesBegan(const NSSet<UITouch*>* touches, const UIEvent* event);
            void TouchesMoved(const NSSet<UITouch*>* touches, const UIEvent* event);
            void TouchesEnded(const NSSet<UITouch*>* touches, const UIEvent* event);
            void TouchesCancelled(const NSSet<UITouch*>* touches, const UIEvent* event);
        #endif

    private:
        std::vector<Touch> activeTouches;

    };

}
