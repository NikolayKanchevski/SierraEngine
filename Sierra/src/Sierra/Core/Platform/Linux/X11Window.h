//
// Created by Nikolay Kanchevski on 8.26.23.
//

#pragma once

#if !SR_PLATFORMLINUX
    #error "Including the X11Window.h file is only allowed in Linux builds!"
#endif

#include "../../Window.h"
#include "LinuxInstance.h"

namespace Sierra
{

    class SIERRA_API X11Window final : public Window
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit X11Window(const WindowCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void OnUpdate() override;
        void Minimize() override;
        void Maximize() override;
        void Show() override;
        void Hide() override;
        void Focus() override;
        void Close() override;

        /* --- SETTER METHODS --- */
        void SetTitle(const String &title) override;
        void SetPosition(const Vector2Int &position) override;
        void SetSize(const Vector2UInt &size) override;
        void SetOpacity(float32 opacity) override;

        /* --- GETTER METHODS --- */
        String GetTitle() const override;
        Vector2Int GetPosition() const override;
        Vector2UInt GetSize() const override;
        Vector2UInt GetFramebufferSize() const override;
        float32 GetOpacity() const override;
        bool IsClosed() const override;
        bool IsMinimized() const override;
        bool IsMaximized() const override;
        bool IsFocused() const override;
        bool IsHidden() const override;
        WindowAPI GetAPI() const override;

        /* --- DESTRUCTOR --- */
        ~X11Window() override;

    private:
        LinuxInstance &linuxInstance;

        XID window;
        String title;
        Vector4Int extents = { 0, 0, 0, 0 }; // [ Left | Right | Top | Bottom ]
        bool closed = false;

        Vector2Int position = { std::numeric_limits<int32>::max(), std::numeric_limits<int32>::max() };
        Vector2UInt size = { std::numeric_limits<uint32>::max(), std::numeric_limits<uint32>::max() };
        bool minimized = false;
        bool maximized = false;

        int32 GetProperty(Atom property, Atom type, uchar** value) const;
        int32 GetRootProperty(Atom property, Atom type, uchar** value) const;
        void RegisterX11Event(const Atom type, const long a = 0, const long b = 0, const long c = 0, const long d = 0, const int eventMask = NoEventMask);
        void HandleX11Event(XEvent &event);

    };

}
