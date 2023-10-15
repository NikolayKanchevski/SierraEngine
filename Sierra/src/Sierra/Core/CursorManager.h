//
// Created by Nikolay Kanchevski on 3.10.23.
//

#pragma once

#include "../Events/CursorEvent.h"

namespace Sierra
{

    struct CursorManagerCreateInfo
    {

    };

    class SIERRA_API CursorManager
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        template<typename T>
        using CursorEventCallback = std::function<bool(const T&)>;

        /* --- CONSTRUCTORS --- */
        explicit CursorManager(const CursorManagerCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        virtual void OnUpdate();

        /* --- SETTER METHODS --- */
        virtual void SetCursorPosition(const Vector2 &position);
        virtual void ShowCursor();
        virtual void HideCursor();

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual Vector2 GetCursorPosition();
        [[nodiscard]] virtual bool IsCursorShown();
        [[nodiscard]] virtual bool IsCursorHidden();

        [[nodiscard]] virtual float GetHorizontalDelta();
        [[nodiscard]] virtual float GetVerticalDelta();

        /* --- EVENTS --- */
        template<typename T> void OnEvent(CursorEventCallback<T>) { static_assert(std::is_base_of_v<CursorEvent, T> && !std::is_same_v<CursorEvent, T>, "Template function accepts derived cursor events only!"); }

        /* --- DESTRUCTORS --- */
        virtual ~CursorManager() = default;

        /* --- OPERATORS --- */
        CursorManager(const CursorManager&) = delete;
        CursorManager& operator=(const CursorManager&) = delete;

    protected:
        [[nodiscard]] inline const EventDispatcher<CursorMoveEvent>& GetCursorMoveDispatcher() const { return cursorMoveDispatcher; }

    private:
        EventDispatcher<CursorMoveEvent> cursorMoveDispatcher;

    };

    template<> inline void CursorManager::OnEvent<CursorMoveEvent>(CursorEventCallback<CursorMoveEvent> Callback) { cursorMoveDispatcher.Subscribe(Callback); }

}
