//
// Created by Nikolay Kanchevski on 30.11.23.
//

#include "AndroidContext.h"

#include <game-activity/native_app_glue/android_native_app_glue.h>

#include "../../Windowing/Android/GameKitWindow.h"

// Define unused entry symbol, so that the game-activity library can be linked to the engine as well (actual entry point is in application)
void android_main(android_app* const) { }

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    AndroidContext::AndroidContext()
        : gameKitContext({ .app = app })
    {

    }

    /* --- POLLING METHODS --- */

    std::unique_ptr<Window> AndroidContext::CreateWindow(const WindowCreateInfo& createInfo) const
    {
        return std::make_unique<GameKitWindow>(gameKitContext, createInfo);
    }

}
