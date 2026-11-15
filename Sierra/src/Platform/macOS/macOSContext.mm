//
// Created by Nikolay Kanchevski on 3.09.23.
//

#include "macOSContext.h"

#include <UniformTypeIdentifiers/UniformTypeIdentifiers.h>

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    macOSContext::macOSContext()
        : windowManager({ }), fileManager({ .fileManager = [NSFileManager defaultManager] })
    {

    }

}
