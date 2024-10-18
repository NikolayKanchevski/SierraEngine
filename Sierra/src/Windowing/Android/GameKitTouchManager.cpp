//
// Created by Nikolay Kanchevski on 12/14/23.
//

#include "GameKitTouchManager.h"

namespace Sierra
{

    /* --- POLLING METHODS --- */

    void GameKitTouchManager::RegisterTouchPress(const Touch& touch)
    {
        TouchManager::RegisterTouchPress(touch);

        const auto iterator = std::find(touches.begin(), touches.end(), touch);
        if (iterator == touches.end())
        {
            touches.emplace_back(touch);
            GetTouchPressDispatcher().DispatchEvent(touch);
        }
    }

    void GameKitTouchManager::RegisterTouchMove(const TouchID ID, const Vector2 position)
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

    void GameKitTouchManager::RegisterTouchRelease(TouchID ID)
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

    /* --- PRIVATE METHODS --- */

    void GameKitTouchManager::Update(const android_input_buffer& inputBuffer)
    {
        std::erase_if(touches, [](const Touch& touch) -> bool { return touch.GetType() == TouchType::Release; });
    }

}