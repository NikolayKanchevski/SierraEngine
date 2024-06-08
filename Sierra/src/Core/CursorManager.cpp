 //
// Created by Nikolay Kanchevski on 3.10.23.
//

#include "CursorManager.h"

namespace Sierra
{


    /* --- POLLING METHODS --- */

    void CursorManager::RegisterCursorMove(const Vector2)
    {

    }

    /* --- SETTER METHODS --- */

    void CursorManager::SetCursorVisibility(const bool visible)
    {

    }

    void CursorManager::SetCursorPosition(const Vector2)
    {

    }

    /* --- GETTER METHODS --- */

    bool CursorManager::IsCursorVisible() const
    {
        return false;
    }

    Vector2 CursorManager::GetCursorPosition() const
    {
        return { std::numeric_limits<float32>::min(), std::numeric_limits<float32>::min() };
    }

    Vector2 CursorManager::GetCursorDelta() const
    {
        return { 0.0f, 0.0f };
    }

}