//
// Created by Nikolay Kanchevski on 6.8.2024.
//

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR, const int nCmdShow)
{
    const std::unique_ptr<Sierra::Application> application = std::unique_ptr<Sierra::Application>(Sierra::CreateApplication(__argc, __argv));
    APP_THROW_IF(application == nullptr, Sierra::InvalidValueError("Cannot create Windows application, as application returned from Sierra::CreateApplication() must not be null"));

    while (!application->Update());
    return WM_QUIT;
}