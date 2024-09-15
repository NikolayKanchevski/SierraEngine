//
// Created by Nikolay Kanchevski on 10.31.23.
//

#pragma once

#include "EventDispatcher.hpp"

namespace Sierra
{

    enum class ScreenOrientation : uint8
    {
        Unknown                   = 0x0000,
        Portrait                  = 0x0001,
        PortraitFlipped           = 0x0002,
        PortraitAny               = Portrait | PortraitFlipped,
        Landscape                 = 0x0004,
        LandscapeFlipped          = 0x0008,
        LandscapeAny              = Landscape | LandscapeFlipped
    };
    SR_DEFINE_ENUM_FLAG_OPERATORS(ScreenOrientation);

    class SIERRA_API ScreenEvent : public Event { protected: ScreenEvent() = default; };
    template<typename T> concept ScreenEventType = std::is_base_of_v<ScreenEvent, T> && !std::is_same_v<ScreenEvent, std::decay_t<T>>;

    class SIERRA_API ScreenReorientEvent : public ScreenEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit ScreenReorientEvent(const ScreenOrientation orientation) : orientation(orientation) { }

        /* --- GETTER METHODS --- */
        [[nodiscard]] ScreenOrientation GetOrientation() const { return orientation; }

    private:
        const ScreenOrientation orientation = ScreenOrientation::Unknown;

    };

    class SIERRA_API Screen
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        template<ScreenEventType EventType>
        using EventCallback = std::function<bool(const EventType&)>;

        /* --- POLLING METHODS --- */
        virtual void RegisterScreenReorient(ScreenOrientation orientation);

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual std::string_view GetName() const = 0;
        [[nodiscard]] virtual ScreenOrientation GetOrientation() const = 0;
        [[nodiscard]] virtual uint16 GetRefreshRate() const = 0;

        [[nodiscard]] virtual Vector2Int GetOrigin() const = 0;
        [[nodiscard]] virtual uint32 GetWidth() const = 0;
        [[nodiscard]] virtual uint32 GetHeight() const = 0;

        [[nodiscard]] virtual Vector2Int GetWorkAreaOrigin() const = 0;
        [[nodiscard]] virtual uint32 GetWorkAreaWidth() const = 0;
        [[nodiscard]] virtual uint32 GetWorkAreaHeight() const = 0;

        /* --- EVENTS --- */
        template<ScreenEventType EventType>
        EventSubscriptionID AddEventListener(const EventCallback<EventType>&);

        template<ScreenEventType EventType>
        bool RemoveEventListener(EventSubscriptionID);

        /* --- COPY SEMANTICS --- */
        Screen(const Screen&) = delete;
        Screen& operator=(const Screen&) = delete;

        /* --- MOVE SEMANTICS --- */
        Screen(Screen&&) = default;
        Screen& operator=(Screen&&) = default;

        /* --- DESTRUCTOR --- */
        virtual ~Screen() = default;

    protected:
        Screen() = default;

        [[nodiscard]] EventDispatcher<ScreenReorientEvent>& GetScreenReorientDispatcher() { return screenReorientDispatcher; }

    private:
        EventDispatcher<ScreenReorientEvent> screenReorientDispatcher;

    };

    template<> inline EventSubscriptionID Screen::AddEventListener<ScreenReorientEvent>(const EventCallback<ScreenReorientEvent>& Callback) { return screenReorientDispatcher.Subscribe(Callback); }
    template<> inline bool Screen::RemoveEventListener<ScreenReorientEvent>(const EventSubscriptionID ID) { return screenReorientDispatcher.Unsubscribe(ID); }

}
