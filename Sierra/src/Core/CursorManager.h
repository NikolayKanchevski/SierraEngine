//
// Created by Nikolay Kanchevski on 3.10.23.
//

#pragma once

#include "EventDispatcher.hpp"

namespace Sierra
{

    class SIERRA_API CursorEvent : public Event { };
    template<typename T> concept CursorEventType = std::is_base_of_v<CursorEvent, T> && !std::is_same_v<CursorEvent, std::decay_t<T>>;

    class SIERRA_API CursorMoveEvent final : public CursorEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        inline explicit CursorMoveEvent(Vector2 position) : position(position) { }

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline const Vector2& GetPosition() const { return position; }

    private:
        Vector2 position;

    };

    struct CursorManagerCreateInfo
    {

    };

    class SIERRA_API CursorManager
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        template<CursorEventType EventType>
        using EventCallback = std::function<bool(const EventType&)>;

        /* --- CONSTRUCTORS --- */
        explicit CursorManager(const CursorManagerCreateInfo &createInfo);

        /* --- SETTER METHODS --- */
        virtual void ShowCursor();
        virtual void HideCursor();
        virtual void SetCursorPosition(Vector2 position);

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual bool IsCursorHidden() const;
        [[nodiscard]] virtual Vector2 GetCursorPosition() const;
        [[nodiscard]] virtual float32 GetHorizontalDelta() const;
        [[nodiscard]] virtual float32 GetVerticalDelta() const;

        /* --- EVENTS --- */
        template<CursorEventType EventType>
        void OnEvent(const EventCallback<EventType> &Callback) { }

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

    template<> inline void CursorManager::OnEvent<CursorMoveEvent>(const EventCallback<CursorMoveEvent> &Callback) { cursorMoveDispatcher.Subscribe(Callback); }

}
