//
// Created by Nikolay Kanchevski on 6.8.2024.
//

#if !defined(WIN32_LEAN_AND_MEAN)
    #define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <cstdio>

namespace
{
    [[nodiscard]] bool IsStandardHandleValid(const DWORD standardHandle) noexcept
    {
        const HANDLE handle = GetStdHandle(standardHandle);
        return handle != nullptr && handle != INVALID_HANDLE_VALUE && GetFileType(handle) != FILE_TYPE_UNKNOWN;
    }

    // Executable is a GUI one, which has no console of its own, so output (such as logs) is written to the console of the terminal it was launched from, if there is one
    void AttachToParentConsole() noexcept
    {
        // Output, which has already been redirected (such as to a file or a pipe), is left where it goes
        const bool outputRedirected = IsStandardHandleValid(STD_OUTPUT_HANDLE);
        const bool errorRedirected = IsStandardHandleValid(STD_ERROR_HANDLE);
        if (outputRedirected && errorRedirected) return;

        if (!AttachConsole(ATTACH_PARENT_PROCESS)) return;

        const HANDLE console = CreateFileW(L"CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
        if (console == INVALID_HANDLE_VALUE) return;

        FILE* stream = nullptr;
        if (!outputRedirected)
        {
            freopen_s(&stream, "CONOUT$", "w", stdout);
            SetStdHandle(STD_OUTPUT_HANDLE, console);
        }

        if (!errorRedirected)
        {
            freopen_s(&stream, "CONOUT$", "w", stderr);
            SetStdHandle(STD_ERROR_HANDLE, console);
        }
    }
}

int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    AttachToParentConsole();

    // NOTE: Arguments are encoded in the code page of the process, which is what narrow strings (such as those of paths) are interpreted in on Windows
    // NOTE: WinMain() is a C function, which is assumed not to throw, so a missing application is reported with an exit code instead
    const std::unique_ptr<Sierra::Application> application = std::unique_ptr<Sierra::Application>(Sierra::CreateApplication(__argc, __argv));
    if (application == nullptr)
    {
        std::fputs("Created application returned from Sierra::CreateApplication() must not be a null pointer!\n", stderr);
        return EXIT_FAILURE;
    }

    while (!application->Update());
    return 0;
}
