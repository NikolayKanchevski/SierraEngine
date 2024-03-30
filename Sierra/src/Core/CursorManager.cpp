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

    void CursorManager::ShowCursor()
    {

    }

    void CursorManager::HideCursor()
    {

    }

    void CursorManager::SetCursorPosition(const Vector2 position)
    {

    }

    /* --- GETTER METHODS --- */

    bool CursorManager::IsCursorHidden() const
    {
        return true;
    }

    Vector2 CursorManager::GetCursorPosition() const
    {
        return { 0.0f, 0.0f };
    }

    float32 CursorManager::GetHorizontalDelta() const
    {
        return 0.0f;
    }

    float32 CursorManager::GetVerticalDelta() const
    {
        return 0.0f;
    }

}