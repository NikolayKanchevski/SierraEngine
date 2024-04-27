//
// Created by Nikolay Kanchevski on 8.10.23.
//

#define UIKIT_TOUCH_MANAGER_IMPLEMENTATION
#include "UIKitTouchManager.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    UIKitTouchManager::UIKitTouchManager(const Sierra::TouchManagerCreateInfo &createInfo)
        : TouchManager(createInfo)
    {

    }

    /* --- POLLING METHODS --- */

    void UIKitTouchManager::Update()
    {
        std::erase_if(activeTouches, [](const Touch &item) { return item.GetType() == TouchType::Release; });
    }

    /* --- GETTER METHODS --- */

    uint32 UIKitTouchManager::GetTouchCount() const
    {
        return static_cast<uint32>(activeTouches.size());
    }

    const Touch& UIKitTouchManager::GetTouch(const uint32 touchIndex) const
    {
        if (touchIndex >= activeTouches.size()) SR_ERROR("Touch index [{0}] out of range! Make sure to use TouchManager::GetTouchCount() and retrieve touches within the returned range.", touchIndex);
        return activeTouches[touchIndex];
    }

    /* --- EVENTS --- */

    #if defined(__OBJC__) && (defined(UIKIT_TOUCH_MANAGER_IMPLEMENTATION) || defined(UIKIT_WINDOW_IMPLEMENTATION))
        void UIKitTouchManager::TouchesBegan(const NSSet<UITouch*>* touches, const UIEvent* event)
        {
            for (UITouch* rawTouch in touches)
            {
                // Get position within the screen and flip Y coordinate
                const Vector2 position = { [rawTouch locationInView: rawTouch.window.rootViewController.view].x, [[UIScreen mainScreen] bounds].size.height - [rawTouch locationInView: rawTouch.window.rootViewController.view].y };
                
                // Create touch
                const Touch touch = Touch({
                    .type = TouchType::Press,
                    .tapTime = TimePoint::Now(),
                    .tapCount = static_cast<uint32>([rawTouch tapCount]),
                    .force = static_cast<float32>([rawTouch force]),
                    .position = position,
                    .deltaPosition = { 0.0f, 0.0f },
                    .ID = rawTouch
                });

                // Try to find the touch in the vector
                auto iterator = std::find(activeTouches.begin(), activeTouches.end(), touch);

                // Add if missing, otherwise overwrite it
                if (iterator == activeTouches.end())
                {
                    activeTouches.push_back(touch);
                    iterator = activeTouches.end() - 1;
                }
                else
                {
                    *iterator = touch;
                }

                // Dispatch events
                GetTouchTapDispatcher().DispatchEvent(*iterator);
            }
        }

        void UIKitTouchManager::TouchesMoved(const NSSet<UITouch*>* touches, const UIEvent* event)
        {
            for (UITouch* rawTouch in touches)
            {
                // Check if touch has been stored before
                auto iterator = std::find_if(activeTouches.begin(), activeTouches.end(), [&rawTouch](const Touch &item) -> bool { return item.GetID() == rawTouch; });
                if (iterator == activeTouches.end()) continue;
                
                // Get position within the screen and flip Y coordinate
                const Vector2 position = { [rawTouch locationInView: rawTouch.window.rootViewController.view].x, [[UIScreen mainScreen] bounds].size.height - [rawTouch locationInView: rawTouch.window.rootViewController.view].y };
                
                // Create touch
                const Touch touch = Touch({
                    .type = TouchType::Press,
                    .tapTime = TimePoint::Now() - iterator->GetHoldDuration(),
                    .tapCount = static_cast<uint32>([rawTouch tapCount]),
                    .force = static_cast<float32>([rawTouch force]),
                    .position = position,
                    .deltaPosition = position - iterator->GetPosition(),
                    .ID = rawTouch
                });

                // Overwrite old touch
                *iterator = touch;

                // Dispatch events
                GetTouchMoveDispatcher().DispatchEvent(*iterator);
            }
        }

        void UIKitTouchManager::TouchesEnded(const NSSet<UITouch*>* touches, const UIEvent* event)
        {
            // Remove ended touches from the vector
            for (UITouch* rawTouch in touches)
            {
                // Check if touch has been stored before
                auto iterator = std::find_if(activeTouches.begin(), activeTouches.end(), [&rawTouch](const Touch &item) -> bool { return item.GetID() == rawTouch; });
                if (iterator == activeTouches.end()) continue;

                // Get position within the screen and flip Y coordinate
                const Vector2 position = { [rawTouch locationInView: rawTouch.window.rootViewController.view].x, [[UIScreen mainScreen] bounds].size.height - [rawTouch locationInView: rawTouch.window.rootViewController.view].y };

                // Create touch
                const Touch touch = Touch({
                    .type = TouchType::Release,
                    .tapTime = TimePoint::Now() - iterator->GetHoldDuration(),
                    .tapCount = static_cast<uint32>([rawTouch tapCount]),
                    .force = static_cast<float32>([rawTouch force]),
                    .position = position,
                    .deltaPosition = position - iterator->GetPosition(),
                    .ID = rawTouch
                });

                // Overwrite old touch
                *iterator = touch;

                // Dispatch events
                GetTouchMoveDispatcher().DispatchEvent(*iterator);
            }
        }

        void UIKitTouchManager::TouchesCancelled(const NSSet<UITouch*>* touches, const UIEvent* event)
        {
            // All touches have been force-discarded (usually because of some system events such as incoming phone calls)
            for (const Touch &touch : activeTouches)
            {
                GetTouchEndDispatcher().DispatchEvent(touch);
            }
            activeTouches.clear();
        }
    #endif

}
