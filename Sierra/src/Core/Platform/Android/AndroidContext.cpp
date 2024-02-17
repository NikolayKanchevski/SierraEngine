//
// Created by Nikolay Kanchevski on 30.11.23.
//

#include "AndroidContext.h"

// Define unused entry symbol, so that the game-activity library can be linked to the engine as well (actual entry point is in application)
void android_main(android_app*) { }

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    AndroidContext::AndroidContext(const PlatformContextCreateInfo &createInfo)
        : PlatformContext(createInfo), activityContext({ .app = app })
    {

    }

}
