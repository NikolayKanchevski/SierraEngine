//
// Created by Nikolay Kanchevski on 10.31.23.
//

#pragma once

#include "WindowingBackendType.h"

#include "ScreenEvents.h"
#include "../Events/EventDispatcher.hpp"

namespace Sierra
{

    template<typename T>
    concept ScreenEventType = std::is_base_of_v<ScreenEvent, T> && !std::is_same_v<ScreenEvent, std::decay_t<T>>;

    class SIERRA_API Screen
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        template<ScreenEventType EventType>
        using EventCallback = std::function<bool(const EventType&)>;

        /* --- POLLING METHODS --- */
        virtual void RegisterScreenReorient(ScreenOrientation orientation);

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual std::string_view GetName() const noexcept = 0;
        [[nodiscard]] virtual ScreenOrientation GetOrientation() const noexcept = 0;
        [[nodiscard]] virtual uint16 GetRefreshRate() const noexcept = 0;

        [[nodiscard]] virtual Vector2Int GetOrigin() const noexcept = 0;
        [[nodiscard]] virtual uint32 GetWidth() const noexcept = 0;
        [[nodiscard]] virtual uint32 GetHeight() const noexcept = 0;

        [[nodiscard]] virtual Vector2Int GetWorkAreaOrigin() const noexcept = 0;
        [[nodiscard]] virtual uint32 GetWorkAreaWidth() const noexcept = 0;
        [[nodiscard]] virtual uint32 GetWorkAreaHeight() const noexcept = 0;

        [[nodiscard]] virtual WindowingBackendType GetBackendType() const noexcept = 0;

        /* --- EVENTS --- */
        template<ScreenEventType EventType>
        EventSubscriptionID AddEventListener(const EventCallback<EventType>&);

        template<ScreenEventType EventType>
        bool RemoveEventListener(EventSubscriptionID) noexcept;

        /* --- COPY SEMANTICS --- */
        Screen(const Screen&) = delete;
        Screen& operator=(const Screen&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~Screen() noexcept = default;

    protected:
        /* --- CONSTRUCTORS --- */
        Screen() noexcept = default;

        /* --- GETTER METHODS --- */
        [[nodiscard]] EventDispatcher<ScreenReorientEvent>& GetScreenReorientDispatcher() noexcept { return screenReorientDispatcher; }

        /* --- MOVE SEMANTICS --- */
        Screen(Screen&&) noexcept = default;
        Screen& operator=(Screen&&) noexcept = default;

    private:
        EventDispatcher<ScreenReorientEvent> screenReorientDispatcher = { };

    };

    template<> inline EventSubscriptionID Screen::AddEventListener<ScreenReorientEvent>(const EventCallback<ScreenReorientEvent>& Callback) { return screenReorientDispatcher.Subscribe(Callback); }
    template<> inline bool Screen::RemoveEventListener<ScreenReorientEvent>(const EventSubscriptionID ID) noexcept { return screenReorientDispatcher.Unsubscribe(ID); }

}
