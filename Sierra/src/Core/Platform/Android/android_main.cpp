//
// Created by Nikolay Kanchevski on 30.11.23.
//

#include <game-activity/GameActivity.h>
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include "../src/Core/Platform/Android/AndroidApplication.h"

extern int main();
void android_main(android_app* application)
{
    // Save application and call application entry point
    Sierra::AndroidApplication::application = application;
    main();
}