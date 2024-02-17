//
// Created by Nikolay Kanchevski on 30.11.23.
//

#include <game-activity/native_app_glue/android_native_app_glue.h>
#include "AndroidContext.h"

extern int main();
void android_main(android_app* app)
{
    // Save application and call application entry point
    Sierra::AndroidContext::app = app;
    main();
}