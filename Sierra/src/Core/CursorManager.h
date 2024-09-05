//
// Created by Nikolay Kanchevski on 3.10.23.
//

#pragma once

#include "EventDispatcher.hpp"

namespace Sierra
{

    class SIERRA_API CursorEvent : public Event { protected: CursorEvent() = default; };
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
        virtual void RegisterCursorMove(Vector2 position) = 0;

        /* --- SETTER METHODS --- */
        virtual void SetCursorVisibility(bool visible) = 0;
        virtual void SetCursorPosition(Vector2 position) = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual bool IsCursorVisible() const = 0;
        [[nodiscard]] virtual Vector2 GetCursorPosition() const = 0;
        [[nodiscard]] virtual Vector2 GetCursorDelta() const = 0;

        /* --- EVENTS --- */
        template<CursorEventType EventType>
        EventSubscriptionID AddEventListener(const EventCallback<EventType>&);

        template<CursorEventType EventType>
        bool RemoveEventListener(EventSubscriptionID);

        /* --- COPY SEMANTICS --- */
        CursorManager(const CursorManager&) = delete;
        CursorManager& operator=(const CursorManager&) = delete;

        /* --- MOVE SEMANTICS --- */
        CursorManager(CursorManager&&) = delete;
        CursorManager& operator=(CursorManager&&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~CursorManager() = default;

    protected:
        CursorManager() = default;

        [[nodiscard]] EventDispatcher<CursorMoveEvent>& GetCursorMoveDispatcher() { return cursorMoveDispatcher; }

    private:
        EventDispatcher<CursorMoveEvent> cursorMoveDispatcher;

    };

    template<> inline EventSubscriptionID CursorManager::AddEventListener<CursorMoveEvent>(const EventCallback<CursorMoveEvent>& Callback) { return cursorMoveDispatcher.Subscribe(Callback); }
    template<> inline bool CursorManager::RemoveEventListener<CursorMoveEvent>(const EventSubscriptionID ID) { return cursorMoveDispatcher.Unsubscribe(ID); }

}
