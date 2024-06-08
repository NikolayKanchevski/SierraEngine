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
        explicit CursorMoveEvent(const Vector2 position) : position(position) { }

        /* --- GETTER METHODS --- */
        [[nodiscard]] const Vector2& GetPosition() const { return position; }

    private:
        Vector2 position = { 0, 0 };

    };

    class SIERRA_API CursorManager
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        template<CursorEventType EventType>
        using EventCallback = std::function<bool(const EventType&)>;

        /* --- POLLING METHODS --- */
        virtual void RegisterCursorMove(Vector2 position);

        /* --- SETTER METHODS --- */
        virtual void SetCursorVisibility(bool visible);
        virtual void SetCursorPosition(Vector2 position);

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual bool IsCursorVisible() const;
        [[nodiscard]] virtual Vector2 GetCursorPosition() const;
        [[nodiscard]] virtual Vector2 GetCursorDelta() const;

        /* --- EVENTS --- */
        template<CursorEventType EventType>
        void OnEvent(const EventCallback<EventType>&) { }

        /* --- OPERATORS --- */
        CursorManager(const CursorManager&) = delete;
        CursorManager& operator=(const CursorManager&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~CursorManager() = default;

    protected:
        explicit CursorManager() = default;

        [[nodiscard]] EventDispatcher<CursorMoveEvent>& GetCursorMoveDispatcher() { return cursorMoveDispatcher; }

    private:
        EventDispatcher<CursorMoveEvent> cursorMoveDispatcher;

    };

    template<> inline void CursorManager::OnEvent<CursorMoveEvent>(const EventCallback<CursorMoveEvent> &Callback) { cursorMoveDispatcher.Subscribe(Callback); }

}
