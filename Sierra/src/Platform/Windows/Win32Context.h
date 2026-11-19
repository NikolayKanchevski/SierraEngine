//
// Created by Nikolay Kanchevski on 11.19.2023.
//

#pragma once

#if !SR_PLATFORM_WINDOWS
    #error "Including the Win32Context.h file is only allowed in Windows builds!"
#endif

#if !defined(WIN32_LEAN_AND_MEAN)
    #define WIN32_LEAN_AND_MEAN
#endif
#if !defined(NOMINMAX)
    #define NOMINMAX
#endif
#include <Windows.h>

#pragma region Windows Defines // Some of these are not present in all Windows API variants, so they are defined manually
    #if !defined(WM_MOUSEHWHEEL)
        #define WM_MOUSEHWHEEL 0x020E
    #endif
    #if !defined(WM_DPICHANGED)
        #define WM_DPICHANGED 0x02E0
    #endif
    #if !defined(GET_XBUTTON_WPARAM)
        #define GET_XBUTTON_WPARAM(w) (HIWORD(w))
    #endif
    #if !defined(USER_DEFAULT_SCREEN_DPI)
        #define USER_DEFAULT_SCREEN_DPI 96
    #endif
#pragma endregion

#pragma region Windows Macros // Windows.h defines macros, which collide with names of Sierra's methods, so they are removed
    #undef CreateWindow
    #undef CreateFile
    #undef CreateDirectory
    #undef CopyFile
    #undef MoveFile
    #undef DeleteFile
    #undef GetUserName
#pragma endregion

#include "../../Windowing/Windows/Win32Screen.h"

namespace Sierra
{

    struct Win32ContextCreateInfo
    {
        HINSTANCE hInstance = nullptr;
    };

    class SIERRA_API Win32Context final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Win32Context(const Win32ContextCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        void Update();
        void ReloadScreens() noexcept;

        void AdjustWindowRectForDPI(RECT& rect, DWORD style, DWORD exStyle, UINT dpi) const noexcept;

        /* --- GETTER METHODS --- */
        [[nodiscard]] HINSTANCE GetHInstance() const noexcept { return hInstance; }
        [[nodiscard]] LPCWSTR GetWindowClassName() const noexcept;

        [[nodiscard]] UINT GetWindowDPI(HWND window) const noexcept;
        [[nodiscard]] UINT GetMonitorDPI(HMONITOR monitor) const noexcept;

        [[nodiscard]] const Win32Screen& GetPrimaryScreen() const noexcept { return screens[0]; } // NOTE: Screens are never empty, as the primary monitor is always treated as one, and it is moved to the front
        [[nodiscard]] const Win32Screen& GetWindowScreen(HWND window) const noexcept;
        [[nodiscard]] std::span<const Win32Screen> GetScreens() const noexcept { return screens; }

        /* --- CONVERSIONS --- */
        [[nodiscard]] static std::wstring UTF8ToWideString(std::string_view string);
        [[nodiscard]] static std::string WideStringToUTF8(std::wstring_view string);

        /* --- COPY SEMANTICS --- */
        Win32Context(const Win32Context&) = delete;
        Win32Context& operator=(const Win32Context&) = delete;

        /* --- MOVE SEMANTICS --- */
        Win32Context(Win32Context&&) = delete;
        Win32Context& operator=(Win32Context&&) = delete;

        /* --- DESTRUCTOR --- */
        ~Win32Context() noexcept;

    private:
        HINSTANCE hInstance = nullptr;

        HICON icon = nullptr;
        HICON smallIcon = nullptr;

        ATOM windowClass = 0;
        ATOM helperWindowClass = 0;
        HWND helperWindow = nullptr;

        // Functions, which are not present on all supported versions of Windows, and are thus loaded at runtime
        HMODULE user32 = nullptr;
        HMODULE shcore = nullptr;
        UINT(WINAPI* getDpiForWindow)(HWND) = nullptr;
        BOOL(WINAPI* adjustWindowRectExForDpi)(LPRECT, DWORD, BOOL, DWORD, UINT) = nullptr;
        HRESULT(WINAPI* getDpiForMonitor)(HMONITOR, int, UINT*, UINT*) = nullptr;

        std::vector<Win32Screen> screens = { };

        void LoadIcons() noexcept;
        void LoadDPIFunctions() noexcept;
        void SetDPIAwareness() const noexcept;

        static LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
        static LRESULT CALLBACK HelperWindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
        static BOOL CALLBACK EnumDisplayMonitorsProc(HMONITOR monitor, HDC hdc, LPRECT monitorRect, LPARAM data);

    };

}
