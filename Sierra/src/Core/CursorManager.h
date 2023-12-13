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

        /* --- SETTER METHODS --- */
        virtual void ShowCursor();
        virtual void HideCursor();
        virtual void SetCursorPosition(const Vector2 &position);

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual bool IsCursorHidden() const;
        [[nodiscard]] virtual Vector2 GetCursorPosition() const;
        [[nodiscard]] virtual float32 GetHorizontalDelta() const;
        [[nodiscard]] virtual float32 GetVerticalDelta() const;

        /* --- EVENTS --- */
        template<typename T> void OnEvent(CursorEventCallback<T>) { static_assert(std::is_base_of_v<CursorEvent, T> && !std::is_same_v<CursorEvent, T>, "Template function accepts derived cursor events only!"); }

        /* --- OPERATORS --- */
        CursorManager(const CursorManager&) = delete;
        CursorManager& operator=(const CursorManager&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~CursorManager() = default;

    protected:
        [[nodiscard]] inline EventDispatcher<CursorMoveEvent>& GetCursorMoveDispatcher() { return cursorMoveDispatcher; }

    private:
        EventDispatcher<CursorMoveEvent> cursorMoveDispatcher;

    };

    template<> inline void CursorManager::OnEvent<CursorMoveEvent>(CursorEventCallback<CursorMoveEvent> Callback) { cursorMoveDispatcher.Subscribe(Callback); }

}
