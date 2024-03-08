//
// Created by Nikolay Kanchevski on 3.10.23.
//

#pragma once

#include "EventDispatcher.hpp"

namespace Sierra
{

    #pragma region Events
        class SIERRA_API CursorEvent : public Event { };

        class SIERRA_API CursorMoveEvent final : public CursorEvent
        {
        public:
            /* --- CONSTRUCTORS --- */
            inline explicit CursorMoveEvent(const Vector2 &position) : position(position) { }

            /* --- GETTER METHODS --- */
            [[nodiscard]] inline const Vector2& GetPosition() const { return position; }

        private:
            Vector2 position;

        };
    #pragma endregion

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
        template<typename T> requires (std::is_base_of_v<CursorEvent, T> && !std::is_same_v<CursorEvent, T>) void OnEvent(CursorEventCallback<T> Callback) { }

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
