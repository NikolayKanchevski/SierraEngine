//
// Created by Nikolay Kanchevski on 27.01.24.
//

#include "Sampler.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    Sampler::Sampler(const SamplerCreateInfo& createInfo)
    {
        SR_THROW_IF(createInfo.name.empty(), InvalidValueError("Cannot create sampler, as specified name must not be empty"));
    }

}