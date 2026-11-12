//
// Created by Nikolay Kanchevski on 10.6.26.
//

#pragma once

#if !SR_PLATFORM_LINUX
    #error "Including the WaylandContext.h file is only allowed in Linux builds!"
#endif

#include <wayland-client.h>
#include <wayland-cursor.h>
#include <xkbcommon/xkbcommon.h>
#include <libdecor-0/libdecor.h>

#include "protocols/xdg-shell-client-protocol.h"
#include "protocols/viewporter-client-protocol.h"
#include "protocols/xdg-activation-v1-client-protocol.h"
#include "protocols/fractional-scale-v1-client-protocol.h"
#include "protocols/cursor-shape-v1-client-protocol.h"
#include "protocols/xdg-decoration-unstable-v1-client-protocol.h"
#include "protocols/relative-pointer-unstable-v1-client-protocol.h"
#include "protocols/pointer-constraints-unstable-v1-client-protocol.h"

#include "WaylandScreen.h"

namespace Sierra
{

    class WaylandWindow;

    struct WaylandContextCreateInfo
    {
        wl_display* display = nullptr;
    };

    class SIERRA_API WaylandContext final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit WaylandContext(const WaylandContextCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        void Update();
        bool Flush() const noexcept;

        void RegisterWindow(WaylandWindow& window);
        void UnregisterWindow(WaylandWindow& window) noexcept;

        /* --- SETTER METHODS --- */
        void SetCursor(bool visible, uint32 scale) const noexcept;

        /* --- GETTER METHODS --- */
        [[nodiscard]] wl_display* GetDisplay() const noexcept { return display; }
        [[nodiscard]] bool IsValid() const noexcept { return compositor != nullptr && wmBase != nullptr; }
        [[nodiscard]] const char* const* GetTag() const noexcept;

        [[nodiscard]] wl_compositor* GetCompositor() const noexcept { return compositor; }
        [[nodiscard]] wl_seat* GetSeat() const noexcept { return seat; }
        [[nodiscard]] wl_pointer* GetPointer() const noexcept { return pointer; }
        [[nodiscard]] wl_keyboard* GetKeyboard() const noexcept { return keyboard; }
        [[nodiscard]] uint32 GetSerial() const noexcept { return serial; }

        [[nodiscard]] xdg_wm_base* GetWmBase() const noexcept { return wmBase; }
        [[nodiscard]] zxdg_decoration_manager_v1* GetDecorationManager() const noexcept { return decorationManager; }
        [[nodiscard]] xdg_activation_v1* GetActivationManager() const noexcept { return activationManager; }
        [[nodiscard]] libdecor* GetLibdecorContext() const noexcept { return libdecorContext; }

        [[nodiscard]] wp_viewporter* GetViewporter() const noexcept { return viewporter; }
        [[nodiscard]] wp_fractional_scale_manager_v1* GetFractionalScaleManager() const noexcept { return fractionalScaleManager; }

        [[nodiscard]] zwp_relative_pointer_manager_v1* GetRelativePointerManager() const noexcept { return relativePointerManager; }
        [[nodiscard]] zwp_pointer_constraints_v1* GetPointerConstraints() const noexcept { return pointerConstraints; }

        [[nodiscard]] const WaylandWindow* GetPointerFocus() const noexcept { return pointerFocus; }
        [[nodiscard]] const WaylandWindow* GetKeyboardFocus() const noexcept { return keyboardFocus; }

        [[nodiscard]] const WaylandScreen* GetScreen(const wl_output* output) const noexcept { return FindScreen(output); }
        [[nodiscard]] std::span<const std::unique_ptr<WaylandScreen>> GetScreens() const noexcept { return screens; }

        /* --- COPY SEMANTICS --- */
        WaylandContext(const WaylandContext&) = delete;
        WaylandContext& operator=(const WaylandContext&) = delete;

        /* --- MOVE SEMANTICS --- */
        WaylandContext(WaylandContext&&) = delete;
        WaylandContext& operator=(WaylandContext&&) = delete;

        /* --- DESTRUCTOR --- */
        ~WaylandContext() noexcept;

    private:
        struct Callbacks;

        wl_display* display = nullptr;
        wl_registry* registry = nullptr;
        bool connectionLost = false;

        wl_compositor* compositor = nullptr;
        wl_shm* shm = nullptr;

        wl_seat* seat = nullptr;
        wl_pointer* pointer = nullptr;
        wl_keyboard* keyboard = nullptr;

        xdg_wm_base* wmBase = nullptr;
        zxdg_decoration_manager_v1* decorationManager = nullptr;
        xdg_activation_v1* activationManager = nullptr;

        wp_viewporter* viewporter = nullptr;
        wp_fractional_scale_manager_v1* fractionalScaleManager = nullptr;
        wp_cursor_shape_manager_v1* cursorShapeManager = nullptr;

        zwp_relative_pointer_manager_v1* relativePointerManager = nullptr;
        zwp_pointer_constraints_v1* pointerConstraints = nullptr;

        libdecor* libdecorContext = nullptr;
        wl_callback* libdecorReadyCallback = nullptr;
        bool libdecorReady = false;

        xkb_context* xkbContext = nullptr;
        xkb_keymap* xkbKeymap = nullptr;

        wl_cursor_theme* cursorTheme = nullptr;
        wl_cursor_theme* cursorThemeHiDPI = nullptr;
        wl_surface* cursorSurface = nullptr;
        wp_cursor_shape_device_v1* cursorShapeDevice = nullptr;

        uint32 serial = 0;
        uint32 pointerEnterSerial = 0;
        Vector2 pendingPointerScroll = { 0, 0 };

        int keyRepeatTimer = -1;
        int32 keyRepeatRate = 0;
        int32 keyRepeatDelay = 0;
        uint32 keyRepeatScancode = 0;

        WaylandWindow* pointerFocus = nullptr;
        WaylandWindow* keyboardFocus = nullptr;

        std::vector<WaylandWindow*> windows = { };
        std::vector<std::unique_ptr<WaylandScreen>> screens = { };

        [[nodiscard]] WaylandWindow* FindWindow(wl_surface* surface) const noexcept;
        [[nodiscard]] WaylandScreen* FindScreen(const wl_output* output) const noexcept;

        void DispatchEvents();
        void StopKeyRepeat() const noexcept;
        void HandleConnectionLoss();

    };

}
