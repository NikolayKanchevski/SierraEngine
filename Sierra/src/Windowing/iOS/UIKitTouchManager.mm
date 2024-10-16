//
// Created by Nikolay Kanchevski on 8.10.23.
//

#include "UIKitTouchManager.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    UIKitTouchManager::UIKitTouchManager()
        : TouchManager()
    {

    }

    /* --- POLLING METHODS --- */

    void UIKitTouchManager::RegisterTouchPress(const Touch& touch)
    {
        if (touch.GetType() != TouchType::Press) return;
        if (std::ranges::find(touches, touch) == touches.end())
        {
            touches.emplace_back(touch);
            GetTouchPressDispatcher().DispatchEvent(touch);
        }
    }

    void UIKitTouchManager::RegisterTouchMove(const TouchID ID, const Vector2 position)
    {
        const auto iterator = std::ranges::find_if(touches, [ID](const Touch& item) -> bool { return item.GetID() == ID; });
        if (iterator == touches.end()) return;

        *iterator = Touch({
            .ID = iterator->GetID(),
            .type = TouchType::Press,
            .tapTime = iterator->GetTapTime(),
            .force = iterator->GetForce(),
            .position = position,
            .lastPosition = iterator->GetPosition()
        });
        GetTouchMoveDispatcher().DispatchEvent(*iterator);
    }

    void UIKitTouchManager::RegisterTouchRelease(const TouchID ID)
    {
        const auto iterator = std::ranges::find_if(touches, [ID](const Touch& item) -> bool { return item.GetID() == ID; });
        if (iterator == touches.end()) return;

        *iterator = Touch({
            .ID = iterator->GetID(),
            .type = TouchType::Release,
            .tapTime = iterator->GetTapTime(),
            .force = iterator->GetForce(),
            .position = iterator->GetPosition(),
            .lastPosition = iterator->GetLastPosition()
        });
        GetTouchReleaseDispatcher().DispatchEvent(*iterator);
    }

    /* --- PRIVATE METHODS --- */

    void UIKitTouchManager::Update()
    {
        std::erase_if(touches, [](const Touch& item) -> bool { return item.GetType() == TouchType::Release; });
    }

}
