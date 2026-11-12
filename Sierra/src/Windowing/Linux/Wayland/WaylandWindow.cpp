//
// Created by Nikolay Kanchevski on 10.8.26.
//

#include "WaylandWindow.h"

namespace Sierra
{

    namespace
    {
        // Identifies which application the window belongs to (same as class hint of X11 windows)
        constexpr const char* APPLICATION_ID = "SierraEngine";

        // Scales are reported as fractions with this denominator
        constexpr uint32 FRACTIONAL_SCALE_DENOMINATOR = 120;

        constexpr std::chrono::milliseconds CONFIGURE_TIMEOUT = std::chrono::milliseconds(2000);

        void CommitFrameState(libdecor_frame* frame, const Vector2UInt size, libdecor_configuration* configuration = nullptr) noexcept
        {
            libdecor_state* const frameState = libdecor_state_new(static_cast<int>(size.x), static_cast<int>(size.y));
            libdecor_frame_commit(frame, frameState, configuration);
            libdecor_state_free(frameState);
        }
    }

    struct WaylandWindow::Callbacks
    {
        /* --- SURFACE --- */

        static void surfaceHandleEnter(void* userData, wl_surface*, wl_output* output)
        {
            WaylandWindow& window = *static_cast<WaylandWindow*>(userData);

            // Screens are also tracked by others on the same connection (libdecor), and entering their outputs is reported too
            if (output == nullptr || wl_proxy_get_tag(reinterpret_cast<wl_proxy*>(output)) != window.waylandContext.GetTag()) return;

            if (std::find(window.outputs.begin(), window.outputs.end(), output) == window.outputs.end()) window.outputs.emplace_back(output);
            window.UpdateBufferScale();
        }

        static void surfaceHandleLeave(void* userData, wl_surface*, wl_output* output)
        {
            WaylandWindow& window = *static_cast<WaylandWindow*>(userData);
            if (std::erase(window.outputs, output) > 0) window.UpdateBufferScale();
        }

        constexpr static wl_surface_listener SURFACE_LISTENER
        {
            .enter = surfaceHandleEnter,
            .leave = surfaceHandleLeave
        };

        static void fractionalScaleHandlePreferredScale(void* userData, wp_fractional_scale_v1*, const uint32_t numerator)
        {
            WaylandWindow& window = *static_cast<WaylandWindow*>(userData);
            if (numerator == 0 || numerator == window.fractionalScaleNumerator) return;

            window.fractionalScaleNumerator = numerator;
            window.UpdateFramebufferSize();
            window.cursorManager.ApplyCursor();
        }

        constexpr static wp_fractional_scale_v1_listener FRACTIONAL_SCALE_LISTENER
        {
            .preferred_scale = fractionalScaleHandlePreferredScale
        };

        /* --- ACTIVATION --- */

        static void activationTokenHandleDone(void* userData, xdg_activation_token_v1* activationToken, const char* token)
        {
            WaylandWindow& window = *static_cast<WaylandWindow*>(userData);
            if (activationToken != window.activationToken) return;

            xdg_activation_v1_activate(window.waylandContext.GetActivationManager(), token, window.surface);
            xdg_activation_token_v1_destroy(window.activationToken);
            window.activationToken = nullptr;
        }

        constexpr static xdg_activation_token_v1_listener ACTIVATION_TOKEN_LISTENER
        {
            .done = activationTokenHandleDone
        };

        /* --- LIBDECOR --- */

        static void libdecorFrameHandleConfigure(libdecor_frame* frame, libdecor_configuration* configuration, void* userData)
        {
            WaylandWindow& window = *static_cast<WaylandWindow*>(userData);

            // Compositor is not obliged to specify state or size, in which case current ones are to be kept
            bool maximized = window.maximized;
            bool fullscreen = window.fullscreen;
            if (libdecor_window_state windowState = LIBDECOR_WINDOW_STATE_NONE; libdecor_configuration_get_window_state(configuration, &windowState))
            {
                maximized = (windowState & LIBDECOR_WINDOW_STATE_MAXIMIZED) != 0;
                fullscreen = (windowState & LIBDECOR_WINDOW_STATE_FULLSCREEN) != 0;
            }

            int width = 0;
            int height = 0;
            Vector2UInt size = !maximized && !fullscreen ? window.floatingSize : window.size;
            if (libdecor_configuration_get_content_size(configuration, frame, &width, &height) && width > 0 && height > 0)
            {
                size = { static_cast<uint32>(width), static_cast<uint32>(height) };
            }

            if (!maximized && !fullscreen) size = window.GetAspectRatioFittedSize(size);
            window.Configure(size, maximized, fullscreen);

            // NOTE: This is what acknowledges the configuration, so it must be done even if nothing has changed
            CommitFrameState(frame, window.size, configuration);
        }

        static void libdecorFrameHandleClose(libdecor_frame*, void* userData)
        {
            WaylandWindow& window = *static_cast<WaylandWindow*>(userData);
            window.closeRequested = true;
        }

        static void libdecorFrameHandleCommit(libdecor_frame*, void* userData)
        {
            // Decorations are drawn onto subsurfaces, whose changes only take effect, once the surface they belong to is committed
            const WaylandWindow& window = *static_cast<WaylandWindow*>(userData);
            wl_surface_commit(window.surface);
        }

        static void libdecorFrameHandleDismissPopup(libdecor_frame*, const char*, void*)
        {

        }

        inline static libdecor_frame_interface LIBDECOR_FRAME_INTERFACE
        {
            .configure = libdecorFrameHandleConfigure,
            .close = libdecorFrameHandleClose,
            .commit = libdecorFrameHandleCommit,
            .dismiss_popup = libdecorFrameHandleDismissPopup
        };

        /* --- XDG SHELL --- */

        static void xdgToplevelHandleConfigure(void* userData, xdg_toplevel*, const int32_t width, const int32_t height, wl_array* states)
        {
            WaylandWindow& window = *static_cast<WaylandWindow*>(userData);

            // NOTE: This only describes what window is to become, and is applied upon configuration of surface
            window.pendingToplevelState = { };
            if (width > 0 && height > 0) window.pendingToplevelState.size = { static_cast<uint32>(width), static_cast<uint32>(height) };

            const uint32_t* const firstState = static_cast<const uint32_t*>(states->data);
            for (uint32 i = 0; i < states->size / sizeof(uint32_t); i++)
            {
                switch (firstState[i])
                {
                    case XDG_TOPLEVEL_STATE_MAXIMIZED:      { window.pendingToplevelState.maximized = true;     break; }
                    case XDG_TOPLEVEL_STATE_FULLSCREEN:     { window.pendingToplevelState.fullscreen = true;    break; }
                    default:                                { break; }
                }
            }
        }

        static void xdgToplevelHandleClose(void* userData, xdg_toplevel*)
        {
            WaylandWindow& window = *static_cast<WaylandWindow*>(userData);
            window.closeRequested = true;
        }

        constexpr static xdg_toplevel_listener XDG_TOPLEVEL_LISTENER
        {
            .configure = xdgToplevelHandleConfigure,
            .close = xdgToplevelHandleClose
        };

        static void xdgSurfaceHandleConfigure(void* userData, xdg_surface* xdgSurface, const uint32_t serial)
        {
            WaylandWindow& window = *static_cast<WaylandWindow*>(userData);
            xdg_surface_ack_configure(xdgSurface, serial);

            // A size of zero leaves it up to the window to decide
            const ToplevelState& state = window.pendingToplevelState;
            const bool floating = !state.maximized && !state.fullscreen;

            Vector2UInt size = state.size;
            if (size.x == 0 || size.y == 0) size = floating ? window.floatingSize : window.size;
            if (floating) size = window.GetAspectRatioFittedSize(size);

            window.Configure(size, state.maximized, state.fullscreen);
        }

        constexpr static xdg_surface_listener XDG_SURFACE_LISTENER
        {
            .configure = xdgSurfaceHandleConfigure
        };
    };

    /* --- CONSTRUCTORS --- */

    WaylandWindow::WaylandWindow(WaylandContext& waylandContext, const WindowCreateInfo& createInfo)
        : Window(createInfo), waylandContext(waylandContext), cursorManager(waylandContext, *this),
            title(createInfo.title), resizable(createInfo.resizable), transparent(createInfo.transparent),
            size(createInfo.width, createInfo.height), framebufferSize(size), floatingSize(size), maximized(createInfo.maximize)
    {
        surface = wl_compositor_create_surface(waylandContext.GetCompositor());
        SR_THROW_IF(surface == nullptr, InvalidOperationError(SR_FORMAT("Cannot create window [{0}], as creation of its Wayland surface failed", createInfo.title)));

        // Connect window instance to surface, so it can be retrieved, when input for the surface is received
        wl_proxy_set_tag(reinterpret_cast<wl_proxy*>(surface), waylandContext.GetTag());
        wl_surface_add_listener(surface, &Callbacks::SURFACE_LISTENER, this);

        // If supported, have compositor report the exact scale window is to be presented at, and map its framebuffer back to its size (scale is otherwise the integer one of screens it is on)
        if (waylandContext.GetFractionalScaleManager() != nullptr && waylandContext.GetViewporter() != nullptr)
        {
            viewport = wp_viewporter_get_viewport(waylandContext.GetViewporter(), surface);
            wp_viewport_set_destination(viewport, static_cast<int32>(size.x), static_cast<int32>(size.y));

            fractionalScale = wp_fractional_scale_manager_v1_get_fractional_scale(waylandContext.GetFractionalScaleManager(), surface);
            wp_fractional_scale_v1_add_listener(fractionalScale, &Callbacks::FRACTIONAL_SCALE_LISTENER, this);
        }

        UpdateOpaqueRegion();
        waylandContext.RegisterWindow(*this);

        Show();
    }

    /* --- POLLING METHODS --- */

    void WaylandWindow::Update()
    {
        if (closed) return;

        inputManager.Update();
        cursorManager.Update();
    }

    void WaylandWindow::Minimize()
    {
        // NOTE: Wayland never tells windows whether they are minimized, so there is no way of knowing if request was honored
        if (libdecorFrame != nullptr) libdecor_frame_set_minimized(libdecorFrame);
        else if (xdgToplevel != nullptr) xdg_toplevel_set_minimized(xdgToplevel);
    }

    void WaylandWindow::Maximize()
    {
        if (libdecorFrame != nullptr) libdecor_frame_set_maximized(libdecorFrame);
        else if (xdgToplevel != nullptr) xdg_toplevel_set_maximized(xdgToplevel);
        else maximized = true; // Window is hidden, so it is to be maximized upon being shown
    }

    void WaylandWindow::Restore()
    {
        // NOTE: Windows can neither tell if they are minimized, nor undo that, so only maximization is restored
        if (libdecorFrame != nullptr) libdecor_frame_unset_maximized(libdecorFrame);
        else if (xdgToplevel != nullptr) xdg_toplevel_unset_maximized(xdgToplevel);
        else maximized = false;
    }

    void WaylandWindow::Show()
    {
        if (closed || libdecorFrame != nullptr || xdgToplevel != nullptr) return;

        // Surfaces must not have any contents by the time they are assigned a role (which can be the case, if window was presented to, while hidden)
        wl_surface_attach(surface, nullptr, 0, 0);
        wl_surface_commit(surface);

        CreateShellObjects();
    }

    void WaylandWindow::Hide()
    {
        if (closed || (libdecorFrame == nullptr && xdgToplevel == nullptr)) return;

        // NOTE: A surface is hidden by taking its role and contents away
        DestroyShellObjects();
        wl_surface_attach(surface, nullptr, 0, 0);
        wl_surface_commit(surface);

        static_cast<void>(waylandContext.Flush());
    }

    void WaylandWindow::RequestAttention()
    {
        RequestActivation(false);
    }

    void WaylandWindow::Focus()
    {
        RequestActivation(true);
    }

    void WaylandWindow::Close()
    {
        if (closed) return;

        closed = true;
        closeRequested = false;
        focused = false;

        // Release cursor, in case it was locked to the window
        cursorManager.SetCursorVisibility(true);
        waylandContext.UnregisterWindow(*this);

        // NOTE: Surface itself is only destroyed along with the window, as swapchains presenting to it may very well outlive this
        DestroyShellObjects();
        wl_surface_attach(surface, nullptr, 0, 0);
        wl_surface_commit(surface);
        static_cast<void>(waylandContext.Flush());

        GetWindowCloseDispatcher().DispatchEvent();
    }

    /* --- SETTER METHODS --- */

    void WaylandWindow::SetTitle(const std::string_view newTitle)
    {
        title = newTitle;

        if (libdecorFrame != nullptr) libdecor_frame_set_title(libdecorFrame, title.c_str());
        else if (xdgToplevel != nullptr) xdg_toplevel_set_title(xdgToplevel, title.c_str());
    }

    void WaylandWindow::SetPosition(const Vector2Int)
    {
        // NOTE: Wayland leaves the placement of windows entirely up to the compositor, so they can neither be moved, nor is their position known
    }

    void WaylandWindow::SetSize(const Vector2UInt newSize)
    {
        // Size of windows, which are not floating, is dictated by the compositor
        if (closed || !IsFloating()) return;
        if (!Resize(newSize)) return;

        if (!resizable) ApplySizeLimits();
        if (libdecorFrame != nullptr && configured) CommitFrameState(libdecorFrame, size);
    }

    void WaylandWindow::SetSizeLimits(const Vector2UInt newMinimumSize, const Vector2UInt newMaximumSize)
    {
        // NOTE: A limit of zero stands for no limit at all
        minimumSize = newMinimumSize;
        maximumSize = newMaximumSize;

        ApplySizeLimits();
        if (libdecorFrame != nullptr && configured) CommitFrameState(libdecorFrame, size);
    }

    void WaylandWindow::SetAspectRatio(const uint32 numerator, const uint32 denominator)
    {
        // NOTE: A ratio of zero stands for no ratio at all
        aspectRatioNumerator = numerator;
        aspectRatioDenominator = denominator;

        if (IsFloating()) SetSize(GetAspectRatioFittedSize(size));
    }

    void WaylandWindow::SetOpacity(const float32)
    {
        SR_WARNING("Wayland: Cannot set opacity of window [{0}], as that is not something Wayland supports", title);
    }

    /* --- GETTER METHODS --- */

    std::string WaylandWindow::GetTitle() const noexcept
    {
        return title;
    }

    float32 WaylandWindow::GetOpacity() const noexcept
    {
        return 1.0f;
    }

    Vector2Int WaylandWindow::GetPosition() const noexcept
    {
        return { 0, 0 };
    }

    uint32 WaylandWindow::GetWidth() const noexcept
    {
        return size.x;
    }

    uint32 WaylandWindow::GetHeight() const noexcept
    {
        return size.y;
    }

    uint32 WaylandWindow::GetFramebufferWidth() const noexcept
    {
        return framebufferSize.x;
    }

    uint32 WaylandWindow::GetFramebufferHeight() const noexcept
    {
        return framebufferSize.y;
    }

    float32 WaylandWindow::GetScaling() const noexcept
    {
        if (fractionalScale != nullptr) return static_cast<float32>(fractionalScaleNumerator) / static_cast<float32>(FRACTIONAL_SCALE_DENOMINATOR);
        return static_cast<float32>(bufferScale);
    }

    bool WaylandWindow::IsVisible() const noexcept
    {
        return configured;
    }

    bool WaylandWindow::IsTransparent() const noexcept
    {
        return transparent;
    }

    bool WaylandWindow::IsMinimized() const noexcept
    {
        // NOTE: Wayland never tells windows whether they are minimized
        return false;
    }

    bool WaylandWindow::IsMaximized() const noexcept
    {
        return maximized;
    }

    bool WaylandWindow::IsFocused() const noexcept
    {
        return focused;
    }

    bool WaylandWindow::IsClosed() const noexcept
    {
        return closed;
    }

    const WaylandScreen* WaylandWindow::GetScreen() const noexcept
    {
        // A window may span multiple screens, in which case the one it has entered last is preferred
        for (auto iterator = outputs.rbegin(); iterator != outputs.rend(); ++iterator)
        {
            if (const WaylandScreen* const screen = waylandContext.GetScreen(*iterator); screen != nullptr) return screen;
        }

        return nullptr;
    }

    /* --- DESTRUCTOR --- */

    WaylandWindow::~WaylandWindow() noexcept
    {
        if (!closed)
        {
            waylandContext.UnregisterWindow(*this);
            DestroyShellObjects();
        }
        cursorManager.UnlockPointer();

        if (fractionalScale != nullptr) wp_fractional_scale_v1_destroy(fractionalScale);
        if (viewport != nullptr)        wp_viewport_destroy(viewport);
        if (surface != nullptr)         wl_surface_destroy(surface);

        static_cast<void>(waylandContext.Flush());
    }

    /* --- PRIVATE METHODS --- */

    void WaylandWindow::CreateShellObjects()
    {
        configured = false;

        if (libdecor* const libdecorContext = waylandContext.GetLibdecorContext(); libdecorContext != nullptr)
        {
            libdecorFrame = libdecor_decorate(libdecorContext, surface, &Callbacks::LIBDECOR_FRAME_INTERFACE, this);
            SR_ERROR_IF(libdecorFrame == nullptr, "Wayland: Could not create libdecor frame for window [{0}]", title);
        }

        if (libdecorFrame != nullptr)
        {
            libdecor_frame_set_app_id(libdecorFrame, APPLICATION_ID);
            libdecor_frame_set_title(libdecorFrame, title.c_str());

            if (!resizable) libdecor_frame_unset_capabilities(libdecorFrame, LIBDECOR_ACTION_RESIZE);
            ApplySizeLimits();

            if (maximized) libdecor_frame_set_maximized(libdecorFrame);
            libdecor_frame_map(libdecorFrame);
        }
        else
        {
            xdgSurface = xdg_wm_base_get_xdg_surface(waylandContext.GetWmBase(), surface);
            xdg_surface_add_listener(xdgSurface, &Callbacks::XDG_SURFACE_LISTENER, this);

            xdgToplevel = xdg_surface_get_toplevel(xdgSurface);
            xdg_toplevel_add_listener(xdgToplevel, &Callbacks::XDG_TOPLEVEL_LISTENER, this);

            xdg_toplevel_set_app_id(xdgToplevel, APPLICATION_ID);
            xdg_toplevel_set_title(xdgToplevel, title.c_str());

            // NOTE: Without libdecor, window can only be decorated by the compositor
            if (waylandContext.GetDecorationManager() != nullptr)
            {
                xdgDecoration = zxdg_decoration_manager_v1_get_toplevel_decoration(waylandContext.GetDecorationManager(), xdgToplevel);
                zxdg_toplevel_decoration_v1_set_mode(xdgDecoration, ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE);
            }

            ApplySizeLimits();
            if (maximized) xdg_toplevel_set_maximized(xdgToplevel);

            // Commit surface without any contents, to have compositor configure it
            wl_surface_commit(surface);
        }

        // Presenting to a window, before compositor has configured it, is an error, so wait for that to happen
        const std::chrono::steady_clock::time_point deadline = std::chrono::steady_clock::now() + CONFIGURE_TIMEOUT;
        while (!configured && std::chrono::steady_clock::now() < deadline)
        {
            if (wl_display_roundtrip(waylandContext.GetDisplay()) == -1) break;
        }
        SR_ERROR_IF(!configured, "Wayland: Compositor did not configure window [{0}] in time", title);
    }

    void WaylandWindow::DestroyShellObjects() noexcept
    {
        if (activationToken != nullptr)
        {
            xdg_activation_token_v1_destroy(activationToken);
            activationToken = nullptr;
        }

        if (libdecorFrame != nullptr)
        {
            libdecor_frame_unref(libdecorFrame);
            libdecorFrame = nullptr;
        }

        if (xdgDecoration != nullptr)
        {
            zxdg_toplevel_decoration_v1_destroy(xdgDecoration);
            xdgDecoration = nullptr;
        }

        if (xdgToplevel != nullptr)
        {
            xdg_toplevel_destroy(xdgToplevel);
            xdgToplevel = nullptr;
        }

        if (xdgSurface != nullptr)
        {
            xdg_surface_destroy(xdgSurface);
            xdgSurface = nullptr;
        }

        configured = false;
    }

    void WaylandWindow::Configure(const Vector2UInt newSize, const bool newMaximized, const bool newFullscreen)
    {
        const bool wasFloating = IsFloating();
        const bool wasMaximized = maximized;

        maximized = newMaximized;
        fullscreen = newFullscreen;
        configured = true;

        if (maximized && !wasMaximized) GetWindowMaximizeDispatcher().DispatchEvent();
        const bool resized = Resize(newSize);

        // Windows, which are not resizable, are locked to whatever size they are floating at
        if (!resizable && (resized || IsFloating() != wasFloating)) ApplySizeLimits();
    }

    bool WaylandWindow::Resize(Vector2UInt newSize)
    {
        newSize = glm::max(newSize, Vector2UInt(1, 1));
        if (IsFloating()) floatingSize = newSize;

        if (newSize == size) return false;
        size = newSize;

        if (viewport != nullptr) wp_viewport_set_destination(viewport, static_cast<int32>(size.x), static_cast<int32>(size.y));
        UpdateFramebufferSize();

        GetWindowResizeDispatcher().DispatchEvent(size.x, size.y);
        return true;
    }

    void WaylandWindow::ApplySizeLimits() const noexcept
    {
        Vector2UInt minimum = minimumSize;
        Vector2UInt maximum = maximumSize;

        // NOTE: Limits are lifted, while window is not floating, as compositor would otherwise be unable to maximize it
        if (!resizable) minimum = maximum = IsFloating() ? size : Vector2UInt(0, 0);

        if (libdecorFrame != nullptr)
        {
            libdecor_frame_set_min_content_size(libdecorFrame, static_cast<int>(minimum.x), static_cast<int>(minimum.y));
            libdecor_frame_set_max_content_size(libdecorFrame, static_cast<int>(maximum.x), static_cast<int>(maximum.y));
        }
        else if (xdgToplevel != nullptr)
        {
            xdg_toplevel_set_min_size(xdgToplevel, static_cast<int32>(minimum.x), static_cast<int32>(minimum.y));
            xdg_toplevel_set_max_size(xdgToplevel, static_cast<int32>(maximum.x), static_cast<int32>(maximum.y));
        }
    }

    void WaylandWindow::RequestActivation(const bool takeFocus)
    {
        xdg_activation_v1* const activationManager = waylandContext.GetActivationManager();
        if (closed || activationManager == nullptr) return;

        // A new request supersedes the one, which may still be pending
        if (activationToken != nullptr) xdg_activation_token_v1_destroy(activationToken);

        activationToken = xdg_activation_v1_get_activation_token(activationManager);
        xdg_activation_token_v1_add_listener(activationToken, &Callbacks::ACTIVATION_TOKEN_LISTENER, this);

        // Compositors only hand focus over, if request can be traced back to the user interacting with the application. They otherwise merely draw attention to the window
        if (takeFocus)
        {
            if (waylandContext.GetSeat() != nullptr) xdg_activation_token_v1_set_serial(activationToken, waylandContext.GetSerial(), waylandContext.GetSeat());
            if (const WaylandWindow* const focusedWindow = waylandContext.GetKeyboardFocus(); focusedWindow != nullptr) xdg_activation_token_v1_set_surface(activationToken, focusedWindow->GetSurface());
        }

        xdg_activation_token_v1_set_app_id(activationToken, APPLICATION_ID);
        xdg_activation_token_v1_commit(activationToken);
    }

    void WaylandWindow::UpdateBufferScale()
    {
        // NOTE: Compositor reports scale directly, when fractional scaling is supported
        if (fractionalScale != nullptr) return;

        // Window is to be sharp on every screen it is on, so the highest scale is used
        int32 newBufferScale = 1;
        for (const wl_output* const output : outputs)
        {
            if (const WaylandScreen* const screen = waylandContext.GetScreen(output); screen != nullptr) newBufferScale = glm::max(newBufferScale, screen->GetScale());
        }

        if (newBufferScale == bufferScale) return;
        bufferScale = newBufferScale;

        wl_surface_set_buffer_scale(surface, bufferScale);
        UpdateFramebufferSize();
        cursorManager.ApplyCursor();
    }

    void WaylandWindow::UpdateFramebufferSize()
    {
        if (fractionalScale != nullptr)
        {
            // NOTE: Protocol dictates, that halves are rounded away from zero
            framebufferSize = (size * fractionalScaleNumerator + FRACTIONAL_SCALE_DENOMINATOR / 2) / FRACTIONAL_SCALE_DENOMINATOR;
        }
        else
        {
            framebufferSize = size * static_cast<uint32>(bufferScale);
        }

        UpdateOpaqueRegion();
    }

    void WaylandWindow::UpdateOpaqueRegion() const noexcept
    {
        if (transparent) return;

        // Lets compositor skip drawing whatever is behind the window
        wl_region* const region = wl_compositor_create_region(waylandContext.GetCompositor());
        if (region == nullptr) return;

        wl_region_add(region, 0, 0, static_cast<int32>(size.x), static_cast<int32>(size.y));
        wl_surface_set_opaque_region(surface, region);
        wl_region_destroy(region);
    }

    Vector2UInt WaylandWindow::GetAspectRatioFittedSize(Vector2UInt targetSize) const noexcept
    {
        if (aspectRatioNumerator == 0 || aspectRatioDenominator == 0 || targetSize.x == 0 || targetSize.y == 0) return targetSize;

        const float32 aspectRatio = static_cast<float32>(targetSize.x) / static_cast<float32>(targetSize.y);
        const float32 targetAspectRatio = static_cast<float32>(aspectRatioNumerator) / static_cast<float32>(aspectRatioDenominator);

        if (aspectRatio < targetAspectRatio) targetSize.y = static_cast<uint32>(static_cast<float32>(targetSize.x) / targetAspectRatio);
        else if (aspectRatio > targetAspectRatio) targetSize.x = static_cast<uint32>(static_cast<float32>(targetSize.y) * targetAspectRatio);

        return targetSize;
    }

    void WaylandWindow::HandlePointerEnter(const Vector2 position)
    {
        // NOTE: Wayland uses -Y coordinates
        cursorManager.RegisterCursorEnter({ static_cast<int32>(position.x), static_cast<int32>(size.y) - static_cast<int32>(position.y) });
    }

    void WaylandWindow::HandlePointerLeave()
    {
        cursorManager.RegisterCursorLeave();
    }

    void WaylandWindow::HandlePointerMotion(const Vector2 position)
    {
        // NOTE: Wayland uses -Y coordinates
        cursorManager.RegisterCursorMove({ static_cast<int32>(position.x), static_cast<int32>(size.y) - static_cast<int32>(position.y) });
    }

    void WaylandWindow::HandlePointerButton(const uint32 buttonCode, const bool pressed)
    {
        const MouseButton mouseButton = WaylandInputManager::ButtonCodeToMouseButton(buttonCode);
        if (pressed) inputManager.RegisterMouseButtonPress(mouseButton);
        else inputManager.RegisterMouseButtonRelease(mouseButton);
    }

    void WaylandWindow::HandlePointerScroll(const Vector2 scroll)
    {
        inputManager.RegisterMouseScroll(scroll);
    }

    void WaylandWindow::HandleKeyboardEnter()
    {
        if (focused) return;

        focused = true;
        GetWindowFocusDispatcher().DispatchEvent(true);
    }

    void WaylandWindow::HandleKeyboardLeave()
    {
        if (!focused) return;
        focused = false;

        // Releases are only reported to the window in focus, so whatever is still held would otherwise remain so forever
        inputManager.ReleaseKeys();
        inputManager.ReleaseMouseButtons();

        GetWindowFocusDispatcher().DispatchEvent(false);
    }

    void WaylandWindow::HandleKey(const uint32 scancode, const bool pressed)
    {
        const Key key = WaylandInputManager::ScancodeToKey(scancode);
        if (pressed) inputManager.RegisterKeyPress(key);
        else inputManager.RegisterKeyRelease(key);
    }

    void WaylandWindow::HandleScreenRemoval(const WaylandScreen& screen)
    {
        if (std::erase(outputs, screen.GetOutput()) > 0) UpdateBufferScale();
    }

}
