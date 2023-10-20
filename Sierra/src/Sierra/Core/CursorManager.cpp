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

    /* --- POLLING METHODS --- */

    void CursorManager::OnUpdate()
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

    bool CursorManager::IsCursorShown()
    {
        return false;
    }

    bool CursorManager::IsCursorHidden()
    {
        return true;
    }

    float32 CursorManager::GetHorizontalDelta()
    {
        return 0.0f;
    }

    float32 CursorManager::GetVerticalDelta()
    {
        return 0.0f;
    }

}