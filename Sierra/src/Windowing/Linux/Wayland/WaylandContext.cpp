//
// Created by Nikolay Kanchevski on 10.6.26.
//

#include "WaylandContext.h"

#include <cmath>
#include <cerrno>
#include <cstring>
#include <poll.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/timerfd.h>

#include "WaylandWindow.h"


namespace Sierra
{

    namespace
    {
        #define SR_STRINGIFY_IMPLEMENTATION(x) #x
        #define SR_STRINGIFY(x) SR_STRINGIFY_IMPLEMENTATION(x)
        #define SR_VERSION_STRING "Sierra " SR_STRINGIFY(SR_VERSION_MAJOR) "." SR_STRINGIFY(SR_VERSION_MINOR) "." SR_STRINGIFY(SR_VERSION_PATCH)

        // Proxies created by the context are tagged, so they can be told apart from those created by others on the same connection (such as libdecor)
        constexpr const char* TAG = SR_VERSION_STRING;

        #undef SR_STRINGIFY_IMPLEMENTATION
        #undef SR_STRINGIFY
        #undef SR_VERSION_STRING

        // Latest versions of the globals, whose events the context is able to handle
        constexpr uint32 COMPOSITOR_VERSION = 3;
        constexpr uint32 SEAT_VERSION = 8;
        constexpr uint32 OUTPUT_VERSION = 4;

        constexpr int DEFAULT_CURSOR_SIZE = 24;
        constexpr uint32 LIBDECOR_INIT_ATTEMPTS = 8;

        template<typename T>
        [[nodiscard]] T* BindGlobal(wl_registry* registry, const uint32 name, const wl_interface& interface, const uint32 version) noexcept
        {
            return static_cast<T*>(wl_registry_bind(registry, name, &interface, version));
        }

        [[nodiscard]] std::string_view LibdecorErrorToString(const libdecor_error error) noexcept
        {
            switch (error)
            {
                case LIBDECOR_ERROR_COMPOSITOR_INCOMPATIBLE:        return "LIBDECOR_ERROR_COMPOSITOR_INCOMPATIBLE";
                case LIBDECOR_ERROR_INVALID_FRAME_CONFIGURATION:    return "LIBDECOR_ERROR_INVALID_FRAME_CONFIGURATION";
            }

            return "Unknown";
        }
    }

    struct WaylandContext::Callbacks
    {
        /* --- SCREENS --- */

        static void outputHandleGeometry(void* userData, wl_output* output, const int32_t x, const int32_t y, int32_t, int32_t, int32_t, const char* make, const char* model, const int32_t transform)
        {
            const WaylandContext& context = *static_cast<WaylandContext*>(userData);

            WaylandScreen* const screen = context.FindScreen(output);
            if (screen == nullptr) return;

            screen->origin = { x, y };
            screen->rotated = transform == WL_OUTPUT_TRANSFORM_90 || transform == WL_OUTPUT_TRANSFORM_270 || transform == WL_OUTPUT_TRANSFORM_FLIPPED_90 || transform == WL_OUTPUT_TRANSFORM_FLIPPED_270;

            // Older compositors do not name their outputs, so fall back to a description of the hardware
            if (screen->name.empty()) screen->name = SR_FORMAT("{0} {1}", make, model);
        }

        static void outputHandleMode(void* userData, wl_output* output, const uint32_t flags, const int32_t width, const int32_t height, const int32_t refresh)
        {
            const WaylandContext& context = *static_cast<WaylandContext*>(userData);

            WaylandScreen* const screen = context.FindScreen(output);
            if (screen == nullptr || !(flags & WL_OUTPUT_MODE_CURRENT)) return;

            // NOTE: Refresh rate is reported in mHz
            screen->size = { static_cast<uint32>(width), static_cast<uint32>(height) };
            screen->refreshRate = static_cast<uint16>(std::lround(static_cast<float64>(refresh) / 1000.0));
        }

        static void outputHandleDone(void*, wl_output*)
        {

        }

        static void outputHandleScale(void* userData, wl_output* output, const int32_t factor)
        {
            const WaylandContext& context = *static_cast<WaylandContext*>(userData);

            WaylandScreen* const screen = context.FindScreen(output);
            if (screen == nullptr) return;

            screen->scale = factor;
            for (WaylandWindow* const window : context.windows)
            {
                window->UpdateBufferScale();
            }
        }

        static void outputHandleName(void* userData, wl_output* output, const char* name)
        {
            const WaylandContext& context = *static_cast<WaylandContext*>(userData);
            if (WaylandScreen* const screen = context.FindScreen(output); screen != nullptr) screen->name = name;
        }

        static void outputHandleDescription(void*, wl_output*, const char*)
        {

        }

        constexpr static wl_output_listener OUTPUT_LISTENER
        {
            .geometry = outputHandleGeometry,
            .mode = outputHandleMode,
            .done = outputHandleDone,
            .scale = outputHandleScale,
            .name = outputHandleName,
            .description = outputHandleDescription
        };

        /* --- POINTER --- */

        static void pointerHandleEnter(void* userData, wl_pointer*, const uint32_t serial, wl_surface* surface, const wl_fixed_t x, const wl_fixed_t y)
        {
            WaylandContext& context = *static_cast<WaylandContext*>(userData);
            context.serial = serial;

            // Surface is either already destroyed, or it is not a window (libdecor draws its decorations onto surfaces of its own)
            WaylandWindow* const window = context.FindWindow(surface);
            if (window == nullptr) return;

            context.pointerEnterSerial = serial;
            context.pointerFocus = window;
            window->HandlePointerEnter({ wl_fixed_to_double(x), wl_fixed_to_double(y) });
        }

        static void pointerHandleLeave(void* userData, wl_pointer*, const uint32_t serial, wl_surface* surface)
        {
            WaylandContext& context = *static_cast<WaylandContext*>(userData);
            context.serial = serial;

            WaylandWindow* const window = context.pointerFocus;
            if (window == nullptr || (surface != nullptr && surface != window->GetSurface())) return;

            context.pointerFocus = nullptr;
            context.pendingPointerScroll = { 0, 0 };
            window->HandlePointerLeave();
        }

        static void pointerHandleMotion(void* userData, wl_pointer*, uint32_t, const wl_fixed_t x, const wl_fixed_t y)
        {
            const WaylandContext& context = *static_cast<WaylandContext*>(userData);
            if (context.pointerFocus != nullptr) context.pointerFocus->HandlePointerMotion({ wl_fixed_to_double(x), wl_fixed_to_double(y) });
        }

        static void pointerHandleButton(void* userData, wl_pointer*, const uint32_t serial, uint32_t, const uint32_t button, const uint32_t state)
        {
            WaylandContext& context = *static_cast<WaylandContext*>(userData);
            context.serial = serial;

            if (context.pointerFocus != nullptr) context.pointerFocus->HandlePointerButton(button, state == WL_POINTER_BUTTON_STATE_PRESSED);
        }

        static void pointerHandleAxis(void* userData, wl_pointer* pointer, uint32_t, const uint32_t axis, const wl_fixed_t value)
        {
            WaylandContext& context = *static_cast<WaylandContext*>(userData);
            if (context.pointerFocus == nullptr) return;

            // NOTE: Wayland reports scrolling down as positive, so vertical scroll is inverted to make scrolling up positive, as on other backends. Horizontal scroll is already positive towards the right
            if (axis == WL_POINTER_AXIS_HORIZONTAL_SCROLL) context.pendingPointerScroll.x += static_cast<float32>(wl_fixed_to_double(value));
            else if (axis == WL_POINTER_AXIS_VERTICAL_SCROLL) context.pendingPointerScroll.y -= static_cast<float32>(wl_fixed_to_double(value));

            // Scrolling along both axes is reported separately, and gets grouped by a frame event (older compositors do not send one)
            if (wl_pointer_get_version(pointer) < WL_POINTER_FRAME_SINCE_VERSION) pointerHandleFrame(userData, pointer);
        }

        static void pointerHandleFrame(void* userData, wl_pointer*)
        {
            WaylandContext& context = *static_cast<WaylandContext*>(userData);
            if (context.pendingPointerScroll == Vector2(0, 0)) return;

            const Vector2 scroll = context.pendingPointerScroll;
            context.pendingPointerScroll = { 0, 0 };
            if (context.pointerFocus != nullptr) context.pointerFocus->HandlePointerScroll(scroll);
        }

        static void pointerHandleAxisSource(void*, wl_pointer*, uint32_t)
        {

        }

        static void pointerHandleAxisStop(void*, wl_pointer*, uint32_t, uint32_t)
        {

        }

        static void pointerHandleAxisDiscrete(void*, wl_pointer*, uint32_t, int32_t)
        {

        }

        static void pointerHandleAxisValue120(void*, wl_pointer*, uint32_t, int32_t)
        {

        }

        constexpr static wl_pointer_listener POINTER_LISTENER
        {
            .enter = pointerHandleEnter,
            .leave = pointerHandleLeave,
            .motion = pointerHandleMotion,
            .button = pointerHandleButton,
            .axis = pointerHandleAxis,
            .frame = pointerHandleFrame,
            .axis_source = pointerHandleAxisSource,
            .axis_stop = pointerHandleAxisStop,
            .axis_discrete = pointerHandleAxisDiscrete,
            .axis_value120 = pointerHandleAxisValue120
        };

        /* --- KEYBOARD --- */

        static void keyboardHandleKeymap(void* userData, wl_keyboard*, const uint32_t format, const int fileDescriptor, const uint32_t size)
        {
            WaylandContext& context = *static_cast<WaylandContext*>(userData);
            if (format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1 || context.xkbContext == nullptr)
            {
                close(fileDescriptor);
                return;
            }

            void* const keymapString = mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fileDescriptor, 0);
            if (keymapString == MAP_FAILED)
            {
                close(fileDescriptor);
                return;
            }

            xkb_keymap* const keymap = xkb_keymap_new_from_string(context.xkbContext, static_cast<const char*>(keymapString), XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS);
            munmap(keymapString, size);
            close(fileDescriptor);

            if (keymap == nullptr)
            {
                SR_ERROR("Wayland: Could not compile keymap");
                return;
            }

            if (context.xkbKeymap != nullptr) xkb_keymap_unref(context.xkbKeymap);
            context.xkbKeymap = keymap;
        }

        static void keyboardHandleEnter(void* userData, wl_keyboard*, const uint32_t serial, wl_surface* surface, wl_array*)
        {
            WaylandContext& context = *static_cast<WaylandContext*>(userData);
            context.serial = serial;

            WaylandWindow* const window = context.FindWindow(surface);
            if (window == nullptr) return;

            context.keyboardFocus = window;
            window->HandleKeyboardEnter();
        }

        static void keyboardHandleLeave(void* userData, wl_keyboard*, const uint32_t serial, wl_surface*)
        {
            WaylandContext& context = *static_cast<WaylandContext*>(userData);
            context.serial = serial;

            WaylandWindow* const window = context.keyboardFocus;
            if (window == nullptr) return;

            context.StopKeyRepeat();
            context.keyboardFocus = nullptr;
            window->HandleKeyboardLeave();
        }

        static void keyboardHandleKey(void* userData, wl_keyboard*, const uint32_t serial, uint32_t, const uint32_t scancode, const uint32_t state)
        {
            WaylandContext& context = *static_cast<WaylandContext*>(userData);
            context.serial = serial;

            WaylandWindow* const window = context.keyboardFocus;
            if (window == nullptr) return;

            const bool pressed = state == WL_KEYBOARD_KEY_STATE_PRESSED;
            if (pressed)
            {
                // Wayland leaves the repetition of held keys up to its clients, so arm a timer for it (not all keys repeat, modifiers for example do not)
                constexpr xkb_keycode_t EVDEV_KEYCODE_OFFSET = 8;
                if (context.keyRepeatTimer != -1 && context.keyRepeatRate > 0 && context.xkbKeymap != nullptr && xkb_keymap_key_repeats(context.xkbKeymap, scancode + EVDEV_KEYCODE_OFFSET))
                {
                    itimerspec timer = { };
                    if (context.keyRepeatRate > 1) timer.it_interval.tv_nsec = 1'000'000'000 / context.keyRepeatRate;
                    else timer.it_interval.tv_sec = 1;

                    // NOTE: A timer with no initial expiration would be disarmed
                    timer.it_value.tv_sec = context.keyRepeatDelay / 1000;
                    timer.it_value.tv_nsec = glm::max((context.keyRepeatDelay % 1000) * 1'000'000, 1);

                    context.keyRepeatScancode = scancode;
                    timerfd_settime(context.keyRepeatTimer, 0, &timer, nullptr);
                }
            }
            else if (scancode == context.keyRepeatScancode)
            {
                context.StopKeyRepeat();
            }

            window->HandleKey(scancode, pressed);
        }

        static void keyboardHandleModifiers(void* userData, wl_keyboard*, const uint32_t serial, uint32_t, uint32_t, uint32_t, uint32_t)
        {
            WaylandContext& context = *static_cast<WaylandContext*>(userData);
            context.serial = serial;
        }

        static void keyboardHandleRepeatInfo(void* userData, wl_keyboard*, const int32_t rate, const int32_t delay)
        {
            WaylandContext& context = *static_cast<WaylandContext*>(userData);
            context.keyRepeatRate = rate;
            context.keyRepeatDelay = delay;
        }

        constexpr static wl_keyboard_listener KEYBOARD_LISTENER
        {
            .keymap = keyboardHandleKeymap,
            .enter = keyboardHandleEnter,
            .leave = keyboardHandleLeave,
            .key = keyboardHandleKey,
            .modifiers = keyboardHandleModifiers,
            .repeat_info = keyboardHandleRepeatInfo
        };

        /* --- SEAT --- */

        static void seatHandleCapabilities(void* userData, wl_seat* seat, const uint32_t capabilities)
        {
            WaylandContext& context = *static_cast<WaylandContext*>(userData);

            if ((capabilities & WL_SEAT_CAPABILITY_POINTER) && context.pointer == nullptr)
            {
                context.pointer = wl_seat_get_pointer(seat);
                wl_pointer_add_listener(context.pointer, &POINTER_LISTENER, &context);

                if (context.cursorShapeManager != nullptr && context.cursorShapeDevice == nullptr) context.cursorShapeDevice = wp_cursor_shape_manager_v1_get_pointer(context.cursorShapeManager, context.pointer);
            }
            else if (!(capabilities & WL_SEAT_CAPABILITY_POINTER) && context.pointer != nullptr)
            {
                if (WaylandWindow* const window = context.pointerFocus; window != nullptr)
                {
                    context.pointerFocus = nullptr;
                    window->HandlePointerLeave();
                }

                if (context.cursorShapeDevice != nullptr)
                {
                    wp_cursor_shape_device_v1_destroy(context.cursorShapeDevice);
                    context.cursorShapeDevice = nullptr;
                }

                if (wl_pointer_get_version(context.pointer) >= WL_POINTER_RELEASE_SINCE_VERSION) wl_pointer_release(context.pointer);
                else wl_pointer_destroy(context.pointer);
                context.pointer = nullptr;
            }

            if ((capabilities & WL_SEAT_CAPABILITY_KEYBOARD) && context.keyboard == nullptr)
            {
                context.keyboard = wl_seat_get_keyboard(seat);
                wl_keyboard_add_listener(context.keyboard, &KEYBOARD_LISTENER, &context);

                // Older compositors do not specify how keys are to be repeated, so use sensible defaults
                if (wl_keyboard_get_version(context.keyboard) < WL_KEYBOARD_REPEAT_INFO_SINCE_VERSION)
                {
                    context.keyRepeatRate = 25;
                    context.keyRepeatDelay = 500;
                }
            }
            else if (!(capabilities & WL_SEAT_CAPABILITY_KEYBOARD) && context.keyboard != nullptr)
            {
                context.StopKeyRepeat();
                if (WaylandWindow* const window = context.keyboardFocus; window != nullptr)
                {
                    context.keyboardFocus = nullptr;
                    window->HandleKeyboardLeave();
                }

                if (wl_keyboard_get_version(context.keyboard) >= WL_KEYBOARD_RELEASE_SINCE_VERSION) wl_keyboard_release(context.keyboard);
                else wl_keyboard_destroy(context.keyboard);
                context.keyboard = nullptr;
            }
        }

        static void seatHandleName(void*, wl_seat*, const char*)
        {

        }

        constexpr static wl_seat_listener SEAT_LISTENER
        {
            .capabilities = seatHandleCapabilities,
            .name = seatHandleName
        };

        /* --- SHELL --- */

        static void wmBaseHandlePing(void*, xdg_wm_base* wmBase, const uint32_t serial)
        {
            xdg_wm_base_pong(wmBase, serial);
        }

        constexpr static xdg_wm_base_listener WM_BASE_LISTENER
        {
            .ping = wmBaseHandlePing
        };

        static void libdecorHandleError(libdecor*, const libdecor_error error, const char* message)
        {
            SR_ERROR("Wayland: libdecor error [{0}] - {1}", LibdecorErrorToString(error), message);
        }

        inline static libdecor_interface LIBDECOR_INTERFACE
        {
            .error = libdecorHandleError
        };

        static void libdecorReadyHandleDone(void* userData, wl_callback* callback, uint32_t)
        {
            WaylandContext& context = *static_cast<WaylandContext*>(userData);
            context.libdecorReady = true;

            wl_callback_destroy(callback);
            context.libdecorReadyCallback = nullptr;
        }

        constexpr static wl_callback_listener LIBDECOR_READY_LISTENER
        {
            .done = libdecorReadyHandleDone
        };

        /* --- REGISTRY --- */

        static void registryHandleGlobal(void* userData, wl_registry* registry, const uint32_t name, const char* interface, const uint32_t version)
        {
            WaylandContext& context = *static_cast<WaylandContext*>(userData);
            const std::string_view interfaceName = interface;

            if (interfaceName == wl_compositor_interface.name)
            {
                if (context.compositor == nullptr) context.compositor = BindGlobal<wl_compositor>(registry, name, wl_compositor_interface, glm::min(version, COMPOSITOR_VERSION));
            }
            else if (interfaceName == wl_shm_interface.name)
            {
                if (context.shm == nullptr) context.shm = BindGlobal<wl_shm>(registry, name, wl_shm_interface, 1);
            }
            else if (interfaceName == wl_output_interface.name)
            {
                if (version < WL_OUTPUT_DONE_SINCE_VERSION)
                {
                    SR_ERROR("Wayland: Could not use output [{0}], as version [{1}] of its interface is unsupported", name, version);
                    return;
                }

                wl_output* const output = BindGlobal<wl_output>(registry, name, wl_output_interface, glm::min(version, OUTPUT_VERSION));
                if (output == nullptr) return;

                context.screens.emplace_back(std::make_unique<WaylandScreen>(WaylandScreenCreateInfo { .output = output, .registryName = name }));
                wl_proxy_set_tag(reinterpret_cast<wl_proxy*>(output), &TAG);
                wl_output_add_listener(output, &OUTPUT_LISTENER, &context);
            }
            else if (interfaceName == wl_seat_interface.name)
            {
                if (context.seat == nullptr)
                {
                    context.seat = BindGlobal<wl_seat>(registry, name, wl_seat_interface, glm::min(version, SEAT_VERSION));
                    wl_seat_add_listener(context.seat, &SEAT_LISTENER, &context);
                }
            }
            else if (interfaceName == xdg_wm_base_interface.name)
            {
                if (context.wmBase == nullptr)
                {
                    context.wmBase = BindGlobal<xdg_wm_base>(registry, name, xdg_wm_base_interface, 1);
                    xdg_wm_base_add_listener(context.wmBase, &WM_BASE_LISTENER, &context);
                }
            }
            else if (interfaceName == zxdg_decoration_manager_v1_interface.name)
            {
                if (context.decorationManager == nullptr) context.decorationManager = BindGlobal<zxdg_decoration_manager_v1>(registry, name, zxdg_decoration_manager_v1_interface, 1);
            }
            else if (interfaceName == xdg_activation_v1_interface.name)
            {
                if (context.activationManager == nullptr) context.activationManager = BindGlobal<xdg_activation_v1>(registry, name, xdg_activation_v1_interface, 1);
            }
            else if (interfaceName == wp_viewporter_interface.name)
            {
                if (context.viewporter == nullptr) context.viewporter = BindGlobal<wp_viewporter>(registry, name, wp_viewporter_interface, 1);
            }
            else if (interfaceName == wp_fractional_scale_manager_v1_interface.name)
            {
                if (context.fractionalScaleManager == nullptr) context.fractionalScaleManager = BindGlobal<wp_fractional_scale_manager_v1>(registry, name, wp_fractional_scale_manager_v1_interface, 1);
            }
            else if (interfaceName == wp_cursor_shape_manager_v1_interface.name)
            {
                if (context.cursorShapeManager == nullptr)
                {
                    context.cursorShapeManager = BindGlobal<wp_cursor_shape_manager_v1>(registry, name, wp_cursor_shape_manager_v1_interface, 1);

                    // Pointer may already be known, if its seat was announced first
                    if (context.pointer != nullptr && context.cursorShapeDevice == nullptr) context.cursorShapeDevice = wp_cursor_shape_manager_v1_get_pointer(context.cursorShapeManager, context.pointer);
                }
            }
            else if (interfaceName == zwp_relative_pointer_manager_v1_interface.name)
            {
                if (context.relativePointerManager == nullptr) context.relativePointerManager = BindGlobal<zwp_relative_pointer_manager_v1>(registry, name, zwp_relative_pointer_manager_v1_interface, 1);
            }
            else if (interfaceName == zwp_pointer_constraints_v1_interface.name)
            {
                if (context.pointerConstraints == nullptr) context.pointerConstraints = BindGlobal<zwp_pointer_constraints_v1>(registry, name, zwp_pointer_constraints_v1_interface, 1);
            }
        }

        static void registryHandleGlobalRemove(void* userData, wl_registry*, const uint32_t name)
        {
            WaylandContext& context = *static_cast<WaylandContext*>(userData);

            // The only globals, which come and go, are screens
            const auto iterator = std::find_if(context.screens.begin(), context.screens.end(), [name](const std::unique_ptr<WaylandScreen>& screen) -> bool { return screen->GetRegistryName() == name; });
            if (iterator == context.screens.end()) return;

            for (WaylandWindow* const window : context.windows)
            {
                window->HandleScreenRemoval(**iterator);
            }
            context.screens.erase(iterator);
        }

        constexpr static wl_registry_listener REGISTRY_LISTENER
        {
            .global = registryHandleGlobal,
            .global_remove = registryHandleGlobalRemove
        };
    };

    /* --- CONSTRUCTORS --- */

    WaylandContext::WaylandContext(const WaylandContextCreateInfo& createInfo)
        : display(createInfo.display)
    {
        xkbContext = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
        SR_ERROR_IF(xkbContext == nullptr, "Wayland: Could not create xkb context, so held keys are not going to be repeated");

        keyRepeatTimer = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);
        SR_ERROR_IF(keyRepeatTimer == -1, "Wayland: Could not create key repeat timer, so held keys are not going to be repeated - {0}", std::strerror(errno));

        registry = wl_display_get_registry(display);
        wl_registry_add_listener(registry, &Callbacks::REGISTRY_LISTENER, this);

        // Retrieve all globals, and then the initial state of each (capabilities of seat, descriptions of screens, etc.)
        wl_display_roundtrip(display);
        wl_display_roundtrip(display);

        SR_ERROR_IF(compositor == nullptr, "Wayland: Compositor does not provide the required [wl_compositor] interface");
        SR_ERROR_IF(wmBase == nullptr, "Wayland: Compositor does not provide the required [xdg_wm_base] interface");
        if (!IsValid()) return;

        // Not all compositors can decorate windows, in which case libdecor does so within the client instead
        libdecorContext = libdecor_new(display, &Callbacks::LIBDECOR_INTERFACE);
        if (libdecorContext != nullptr)
        {
            // libdecor retrieves its own globals asynchronously, and is not usable beforehand. A sync point, requested after it has requested its own, is how to know when that is done
            libdecor_dispatch(libdecorContext, 0);

            libdecorReadyCallback = wl_display_sync(display);
            wl_callback_add_listener(libdecorReadyCallback, &Callbacks::LIBDECOR_READY_LISTENER, this);
            for (uint32 attempt = 0; attempt < LIBDECOR_INIT_ATTEMPTS && !libdecorReady; attempt++)
            {
                if (wl_display_roundtrip(display) == -1) break;
            }
            SR_WARNING_IF(!libdecorReady, "Wayland: libdecor did not finish initializing, so windows may not be decorated");
        }
        else
        {
            SR_WARNING("Wayland: Could not initialize libdecor, so windows are only going to be decorated, if compositor is able to do so");
        }

        // Load cursor theme (pointer does not carry an image of its own, when hovering windows)
        if (shm != nullptr && compositor != nullptr)
        {
            int cursorSize = DEFAULT_CURSOR_SIZE;
            if (const char* const cursorSizeString = std::getenv("XCURSOR_SIZE"); cursorSizeString != nullptr)
            {
                const long parsedCursorSize = std::strtol(cursorSizeString, nullptr, 10);
                if (parsedCursorSize > 0 && parsedCursorSize < std::numeric_limits<int16>::max())
                {
                    cursorSize = static_cast<int>(parsedCursorSize);
                }
            }

            const char* const cursorThemeName = std::getenv("XCURSOR_THEME");
            cursorTheme = wl_cursor_theme_load(cursorThemeName, cursorSize, shm);
            SR_ERROR_IF(cursorTheme == nullptr, "Wayland: Could not load cursor theme");

            cursorThemeHiDPI = wl_cursor_theme_load(cursorThemeName, cursorSize * 2, shm);
            cursorSurface = wl_compositor_create_surface(compositor);
        }
    }

    /* --- POLLING METHODS --- */

    void WaylandContext::Update()
    {
        if (!connectionLost) DispatchEvents();

        // Windows are not closed from within events (they may be in the middle of being handled by the very objects, which closing would destroy), but once all have been handled
        bool windowClosed = true;
        while (windowClosed)
        {
            windowClosed = false;
            for (WaylandWindow* const window : windows)
            {
                if (!window->closeRequested) continue;

                // Closing unregisters window, so iteration must start over
                window->Close();
                windowClosed = true;
                break;
            }
        }
    }

    bool WaylandContext::Flush() const noexcept
    {
        while (wl_display_flush(display) == -1)
        {
            if (errno != EAGAIN) return false;

            // Outgoing buffer is full, so wait until compositor has read some of it
            pollfd descriptor = { .fd = wl_display_get_fd(display), .events = POLLOUT };
            while (poll(&descriptor, 1, -1) == -1)
            {
                if (errno != EINTR && errno != EAGAIN) return false;
            }
        }

        return true;
    }

    void WaylandContext::RegisterWindow(WaylandWindow& window)
    {
        windows.emplace_back(&window);
    }

    void WaylandContext::UnregisterWindow(WaylandWindow& window) noexcept
    {
        if (pointerFocus == &window) pointerFocus = nullptr;
        if (keyboardFocus == &window)
        {
            keyboardFocus = nullptr;
            StopKeyRepeat();
        }

        std::erase(windows, &window);
    }

    /* --- SETTER METHODS --- */

    void WaylandContext::SetCursor(const bool visible, const uint32 scale) const noexcept
    {
        if (pointer == nullptr) return;

        if (!visible)
        {
            wl_pointer_set_cursor(pointer, pointerEnterSerial, nullptr, 0, 0);
            return;
        }

        // Compositor draws the default cursor itself, at the size and scale it is configured with, which is what makes it match the cursor of the rest of the desktop
        if (cursorShapeDevice != nullptr)
        {
            wp_cursor_shape_device_v1_set_shape(cursorShapeDevice, pointerEnterSerial, WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_DEFAULT);
            return;
        }

        // Pick theme, which best matches the scale of what cursor is over
        const bool useHiDPITheme = scale > 1 && cursorThemeHiDPI != nullptr;
        wl_cursor_theme* const theme = useHiDPITheme ? cursorThemeHiDPI : cursorTheme;
        if (theme == nullptr || cursorSurface == nullptr) return;

        wl_cursor* cursor = wl_cursor_theme_get_cursor(theme, "default");
        if (cursor == nullptr) cursor = wl_cursor_theme_get_cursor(theme, "left_ptr");
        if (cursor == nullptr || cursor->image_count == 0) return;

        wl_cursor_image* const image = cursor->images[0];
        wl_buffer* const buffer = wl_cursor_image_get_buffer(image);
        if (buffer == nullptr) return;

        const int32 imageScale = useHiDPITheme ? 2 : 1;
        wl_pointer_set_cursor(pointer, pointerEnterSerial, cursorSurface, static_cast<int32>(image->hotspot_x) / imageScale, static_cast<int32>(image->hotspot_y) / imageScale);
        wl_surface_set_buffer_scale(cursorSurface, imageScale);
        wl_surface_attach(cursorSurface, buffer, 0, 0);
        wl_surface_damage(cursorSurface, 0, 0, static_cast<int32>(image->width), static_cast<int32>(image->height));
        wl_surface_commit(cursorSurface);
    }

    /* --- GETTER METHODS --- */

    const char* const* WaylandContext::GetTag() const noexcept
    {
        return &TAG;
    }

    /* --- DESTRUCTOR --- */

    WaylandContext::~WaylandContext() noexcept
    {
        if (libdecorReadyCallback != nullptr)   wl_callback_destroy(libdecorReadyCallback);
        if (libdecorContext != nullptr)         libdecor_unref(libdecorContext);

        screens.clear();

        if (xkbKeymap != nullptr)               xkb_keymap_unref(xkbKeymap);
        if (xkbContext != nullptr)              xkb_context_unref(xkbContext);

        if (cursorShapeDevice != nullptr)       wp_cursor_shape_device_v1_destroy(cursorShapeDevice);
        if (cursorSurface != nullptr)           wl_surface_destroy(cursorSurface);
        if (cursorThemeHiDPI != nullptr)        wl_cursor_theme_destroy(cursorThemeHiDPI);
        if (cursorTheme != nullptr)             wl_cursor_theme_destroy(cursorTheme);

        if (pointer != nullptr)
        {
            if (wl_pointer_get_version(pointer) >= WL_POINTER_RELEASE_SINCE_VERSION) wl_pointer_release(pointer);
            else wl_pointer_destroy(pointer);
        }
        if (keyboard != nullptr)
        {
            if (wl_keyboard_get_version(keyboard) >= WL_KEYBOARD_RELEASE_SINCE_VERSION) wl_keyboard_release(keyboard);
            else wl_keyboard_destroy(keyboard);
        }
        if (seat != nullptr)
        {
            if (wl_seat_get_version(seat) >= WL_SEAT_RELEASE_SINCE_VERSION) wl_seat_release(seat);
            else wl_seat_destroy(seat);
        }

        if (pointerConstraints != nullptr)      zwp_pointer_constraints_v1_destroy(pointerConstraints);
        if (relativePointerManager != nullptr)  zwp_relative_pointer_manager_v1_destroy(relativePointerManager);
        if (fractionalScaleManager != nullptr)  wp_fractional_scale_manager_v1_destroy(fractionalScaleManager);
        if (cursorShapeManager != nullptr)      wp_cursor_shape_manager_v1_destroy(cursorShapeManager);
        if (viewporter != nullptr)              wp_viewporter_destroy(viewporter);
        if (activationManager != nullptr)       xdg_activation_v1_destroy(activationManager);
        if (decorationManager != nullptr)       zxdg_decoration_manager_v1_destroy(decorationManager);
        if (wmBase != nullptr)                  xdg_wm_base_destroy(wmBase);
        if (shm != nullptr)                     wl_shm_destroy(shm);
        if (compositor != nullptr)              wl_compositor_destroy(compositor);
        if (registry != nullptr)                wl_registry_destroy(registry);

        if (keyRepeatTimer != -1)               close(keyRepeatTimer);

        if (display != nullptr)
        {
            wl_display_flush(display);
            wl_display_disconnect(display);
        }
    }

    /* --- PRIVATE METHODS --- */

    WaylandWindow* WaylandContext::FindWindow(wl_surface* surface) const noexcept
    {
        if (surface == nullptr || wl_proxy_get_tag(reinterpret_cast<wl_proxy*>(surface)) != &TAG) return nullptr;

        // NOTE: Surfaces of closed windows are kept alive until their destruction, but are of no interest anymore
        WaylandWindow* const window = static_cast<WaylandWindow*>(wl_surface_get_user_data(surface));
        return window != nullptr && !window->closed ? window : nullptr;
    }

    WaylandScreen* WaylandContext::FindScreen(const wl_output* output) const noexcept
    {
        for (const std::unique_ptr<WaylandScreen>& screen : screens)
        {
            if (screen->GetOutput() == output) return screen.get();
        }

        return nullptr;
    }

    void WaylandContext::DispatchEvents()
    {
        // NOTE: Dispatching a broken connection, which still has events queued, would never return
        if (wl_display_get_error(display) != 0)
        {
            HandleConnectionLoss();
            return;
        }

        // Besides its own events, libdecor also processes those of the toolkit its decorations are drawn with, so it is always dispatched
        if (libdecorContext != nullptr) libdecor_dispatch(libdecorContext, 0);

        // Reading new events is only allowed, once all that have already been read are dispatched
        while (wl_display_prepare_read(display) != 0)
        {
            if (wl_display_dispatch_pending(display) == -1)
            {
                HandleConnectionLoss();
                return;
            }
        }

        if (!Flush())
        {
            wl_display_cancel_read(display);
            HandleConnectionLoss();
            return;
        }

        // NOTE: Negative file descriptors (key repeat timer could be one, if its creation has failed) are ignored
        enum : uint8 { DISPLAY_DESCRIPTOR, KEY_REPEAT_DESCRIPTOR, DESCRIPTOR_COUNT };
        std::array<pollfd, DESCRIPTOR_COUNT> descriptors = { };
        descriptors[DISPLAY_DESCRIPTOR] = { .fd = wl_display_get_fd(display), .events = POLLIN };
        descriptors[KEY_REPEAT_DESCRIPTOR] = { .fd = keyRepeatTimer, .events = POLLIN };

        if (poll(descriptors.data(), descriptors.size(), 0) <= 0)
        {
            wl_display_cancel_read(display);
            return;
        }

        if (descriptors[DISPLAY_DESCRIPTOR].revents & (POLLIN | POLLERR | POLLHUP))
        {
            if (wl_display_read_events(display) == -1 || wl_display_dispatch_pending(display) == -1)
            {
                HandleConnectionLoss();
                return;
            }
        }
        else
        {
            wl_display_cancel_read(display);
        }

        if (descriptors[KEY_REPEAT_DESCRIPTOR].revents & POLLIN)
        {
            // Timer reports how many times it has expired since it was last read
            uint64 repeatCount = 0;
            if (read(keyRepeatTimer, &repeatCount, sizeof(repeatCount)) == sizeof(repeatCount))
            {
                for (uint64 i = 0; i < repeatCount && keyboardFocus != nullptr; i++)
                {
                    keyboardFocus->HandleKey(keyRepeatScancode, true);
                }
            }
        }
    }

    void WaylandContext::StopKeyRepeat() const noexcept
    {
        if (keyRepeatTimer == -1) return;

        constexpr itimerspec DISARMED_TIMER = { };
        timerfd_settime(keyRepeatTimer, 0, &DISARMED_TIMER, nullptr);
    }

    void WaylandContext::HandleConnectionLoss()
    {
        if (connectionLost) return;
        connectionLost = true;

        #if SR_ENABLE_LOGGING
            if (const int error = wl_display_get_error(display); error == EPROTO)
            {
                uint32 objectID = 0;
                const wl_interface* interface = nullptr;
                const uint32 code = wl_display_get_protocol_error(display, &interface, &objectID);
                SR_ERROR("Wayland: Connection to compositor was terminated, due to protocol error [{0}] on object [{1}] of interface [{2}]", code, objectID, interface != nullptr ? interface->name : "unknown");
            }
            else
            {
                SR_ERROR("Wayland: Lost connection to compositor - {0}", std::strerror(error != 0 ? error : errno));
            }
        #endif

        // Nothing can be presented without a compositor, so all windows are to be closed
        StopKeyRepeat();
        for (WaylandWindow* const window : windows)
        {
            // TODO: WHY
            window->closeRequested = true;
        }
    }

}
