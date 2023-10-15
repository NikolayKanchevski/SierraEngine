//
// Created by Nikolay Kanchevski on 8.10.23.
//

#include "UIKitTouchManager.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    UIKitTouchManager::UIKitTouchManager(const Sierra::TouchManagerCreateInfo &createInfo)
        : TouchManager(createInfo)
    {

    }

    /* --- GETTER METHODS --- */

    uint32 UIKitTouchManager::GetTouchCount()
    {
        return static_cast<uint32>(activeTouches.size());
    }

    std::optional<Touch> UIKitTouchManager::GetTouch(const uint32 touchIndex)
    {
        if (activeTouches.size() < touchIndex + 1) 
        {
            return std::nullopt;
        }
        
        return activeTouches[touchIndex];
    }

    /* --- EVENTS --- */

    #if defined(__OBJC__)
        void UIKitTouchManager::TouchesBegan(const NSSet<UITouch*>* touches, const UIEvent* event)
        {
            for (UITouch* rawTouch in touches)
            {
                // Get position within the screen and flip Y coordinate
                const Vector2 position = { [rawTouch locationInView: rawTouch.window.rootViewController.view].x, [[UIScreen mainScreen] bounds].size.height - [rawTouch locationInView: rawTouch.window.rootViewController.view].y };
                
                // Create touch
                const Touch touch = Touch({
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
                auto iterator = std::find_if(activeTouches.begin(), activeTouches.end(), [&rawTouch](const Touch &item) { return item.GetID() == rawTouch; });
                if (iterator == activeTouches.end()) continue;
                
                // Get position within the screen and flip Y coordinate
                const Vector2 position = { [rawTouch locationInView: rawTouch.window.rootViewController.view].x, [[UIScreen mainScreen] bounds].size.height - [rawTouch locationInView: rawTouch.window.rootViewController.view].y };
                
                // Create touch
                const Touch touch = Touch({
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
            for (const UITouch* rawTouch in touches)
            {
                activeTouches.erase(std::remove_if(activeTouches.begin(), activeTouches.end(), [&rawTouch, this](const Touch &item)
                {
                    if (item.GetID() == rawTouch)
                    {
                        GetTouchEndDispatcher().DispatchEvent(item);
                        return true;
                    }
                    return false;
                }), activeTouches.end());
            }
        }

        void UIKitTouchManager::TouchesCancelled(const NSSet<UITouch*>* touches, const UIEvent* event)
        {
            // All touches have been force-discarded (usually because of some system events such as incoming phone calls)
            for (const auto &touch : activeTouches)
            {
                GetTouchEndDispatcher().DispatchEvent(touch);
            }
            activeTouches.clear();
        }
    #endif

}
