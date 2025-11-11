//
// Created by Nikolay Kanchevski on 27.10.25.
//

#include "DestructionScheduler.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    DestructionScheduler::DestructionScheduler(const DestructionSchedulerCreateInfo& createInfo)
    {
        SR_THROW_IF(createInfo.name.empty(), InvalidValueError("Cannot create destruction scheduler, as specified name must not be empty"));
    }

}