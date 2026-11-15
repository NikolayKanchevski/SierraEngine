//
// Created by Nikolay Kanchevski on 17.10.24.
//

#include <game-activity/native_app_glue/android_native_app_glue.h>

#include "AndroidContext.h"

void android_main(android_app* const app)
{
    Sierra::AndroidContext::app = app;

    const std::unique_ptr<Sierra::Application> application = std::unique_ptr<Sierra::Application>(Sierra::CreateApplication(0, nullptr));
    1

    while (!application->Update());
}