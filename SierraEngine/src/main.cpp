//
// Created by Nikolay Kanchevski on 18.02.24.
//

#include "Core/Application.h"
using namespace SierraEngine;

int main()
{
    Application application = Application({ .name = "Sierra Engine", .version = Sierra::Version({ 1, 0, 0 }) });
    application.Run();
}