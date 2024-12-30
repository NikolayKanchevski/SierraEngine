//
// Created by Nikolay Kanchevski on 8.10.23.
//

#include "UIKitTouchManager.h"

namespace Sierra
{

    /* --- POLLING METHODS --- */

    void UIKitTouchManager::RegisterTouchPress(const Touch& touch)
    {
        TouchManager::RegisterTouchPress(touch);

        if (std::find(touches.begin(), touches.end(), touch) == touches.end())
        {
            touches.emplace_back(touch);
            GetTouchPressDispatcher().DispatchEvent(touch);
        }
    }

    void UIKitTouchManager::RegisterTouchMove(const TouchID ID, const Vector2 position)
    {
        TouchManager::RegisterTouchMove(ID, position);
        const auto iterator = std::find_if(touches.begin(), touches.end(), [ID](const Touch& touch) -> bool { return touch.GetID() == ID; });

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
        TouchManager::RegisterTouchRelease(ID);
        const auto iterator = std::find_if(touches.begin(), touches.end(), [ID](const Touch& touch) -> bool { return touch.GetID() == ID; });

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

    /* --- POLLING METHODS --- */

    void UIKitTouchManager::Update()
    {
        std::erase_if(touches, [](const Touch& item) -> bool { return item.GetType() == TouchType::Release; });
    }

}
