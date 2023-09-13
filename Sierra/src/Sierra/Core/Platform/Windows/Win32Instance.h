//
// Created by Nikolay Kanchevski on 9.2.2023.
//

#pragma once

#include <Windows.h>
#if defined(CreateWindow)
    #undef CreateWindow
#endif

namespace Sierra
{

    class SIERRA_API Win32Instance
    {
    public:
        /* --- CONSTRUCTORS --- */
        Win32Instance();
        static UniquePtr<Win32Instance> Create();

        /* --- POLLING METHODS --- */
        HWND CreateWindow(LPCSTR title, LONG xPosition, LONG yPosition, ulong width, ulong height, bool resizable, bool maximize, const WNDPROC &proc);
        void CloseWindow(const HWND window);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline HINSTANCE GetHInstance() const { return hInstance; }
        [[nodiscard]] inline int16 GetWindowTitleBarHeight() { return GetSystemMetrics(SM_CYSIZEFRAME) + GetSystemMetrics(SM_CYEDGE) * 2; }

        [[nodiscard]] DWORD GetWindowStyle(const HWND window);
        [[nodiscard]] bool IsWindowsVersionOrGreater(DWORD major, DWORD minor, DWORD servicePack) const;

        /* --- DESTRUCTOR --- */
        ~Win32Instance();

    private:
        HINSTANCE hInstance;
        int16 windowTitleBarHeight;

    };

    #define IsWindows7OrGreater() IsWindowsVersionOrGreater(6, 1, 0)
    #define IsWindows10Version1607OrGreater() IsWindowsVersionOrGreater(10, 0, 14393)

}
