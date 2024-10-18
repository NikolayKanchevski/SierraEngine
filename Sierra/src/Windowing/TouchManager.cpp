//
// Created by Nikolay Kanchevski on 3.10.23.
//

#include "TouchManager.h"

namespace Sierra
{

    /* --- POLLING METHODS --- */

    void TouchManager::RegisterTouchPress(const Touch& touch)
    {
        SR_THROW_IF(touch.GetType() != TouchType::Press, InvalidValueError("Cannot register touch press, as type of specified touch differs from [TouchType::Press]"));
    }

    void TouchManager::RegisterTouchMove(const TouchID ID, const Vector2)
    {
        SR_THROW_IF(!TouchExists(ID), InvalidValueError("Cannot register touch move, as specified touch ID is invalid - use TouchManager::TouchExists() to query existence"));
    }

    void TouchManager::RegisterTouchRelease(const TouchID ID)
    {
        SR_THROW_IF(!TouchExists(ID), InvalidValueError("Cannot register touch release, as specified touch ID is invalid - use TouchManager::TouchExists() to query existence"));
    }

    /* --- GETTER METHODS --- */

    bool TouchManager::TouchExists(const TouchID ID) const noexcept
    {
        const std::span<const Touch> touches = GetTouches();
        auto iterator = std::find_if(touches.begin(), touches.end(), [ID](const Touch& touch) -> bool { return touch.GetID() == ID; });
        return iterator != touches.end();
    }

}
