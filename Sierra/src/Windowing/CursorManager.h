//
// Created by Nikolay Kanchevski on 3.10.23.
//

#pragma once

#include "WindowingBackendType.h"

#include "CursorEvents.h"
#include "../Events/EventDispatcher.hpp"

namespace Sierra
{

    template<typename T>
    concept CursorEventType = std::is_base_of_v<CursorEvent, T> && !std::is_same_v<CursorEvent, std::decay_t<T>>;

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
        [[nodiscard]] virtual bool IsCursorVisible() const noexcept = 0;
        [[nodiscard]] virtual Vector2 GetCursorPosition() const noexcept = 0;
        [[nodiscard]] virtual Vector2 GetCursorDelta() const noexcept = 0;
        [[nodiscard]] virtual WindowingBackendType GetBackendType() const noexcept = 0;

        /* --- EVENTS --- */
        template<CursorEventType EventType>
        EventSubscriptionID AddEventListener(const EventCallback<EventType>&);

        template<CursorEventType EventType>
        bool RemoveEventListener(EventSubscriptionID) noexcept;

        /* --- COPY SEMANTICS --- */
        CursorManager(const CursorManager&) = delete;
        CursorManager& operator=(const CursorManager&) = delete;

        /* --- MOVE SEMANTICS --- */
        CursorManager(CursorManager&&) = delete;
        CursorManager& operator=(CursorManager&&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~CursorManager() noexcept = default;

    protected:
        /* --- CONSTRUCTORS --- */
        CursorManager() noexcept = default;

        /* --- GETTER METHODS --- */
        [[nodiscard]] EventDispatcher<CursorMoveEvent>& GetCursorMoveDispatcher() noexcept { return cursorMoveDispatcher; }

    private:
        EventDispatcher<CursorMoveEvent> cursorMoveDispatcher = { };

    };

    template<> inline EventSubscriptionID CursorManager::AddEventListener<CursorMoveEvent>(const EventCallback<CursorMoveEvent>& Callback) { return cursorMoveDispatcher.Subscribe(Callback); }
    template<> inline bool CursorManager::RemoveEventListener<CursorMoveEvent>(const EventSubscriptionID ID) noexcept { return cursorMoveDispatcher.Unsubscribe(ID); }

}
