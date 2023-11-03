//
// Created by Nikolay Kanchevski on 3.10.23.
//

#include "CursorManager.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    CursorManager::CursorManager(const CursorManagerCreateInfo &createInfo)
    {

    }

    /* --- SETTER METHODS --- */

    void CursorManager::SetCursorPosition(const Vector2 &position)
    {

    }

    void CursorManager::ShowCursor()
    {

    }

    void CursorManager::HideCursor()
    {

    }

    /* --- GETTER METHODS --- */

    Vector2 CursorManager::GetCursorPosition()
    {
        return { std::numeric_limits<float32>::min(), std::numeric_limits<float32>::min() };
    }

    float32 CursorManager::GetHorizontalDelta()
    {
        return 0.0f;
    }

    float32 CursorManager::GetVerticalDelta()
    {
        return 0.0f;
    }

    bool CursorManager::IsCursorHidden()
    {
        return true;
    }

}