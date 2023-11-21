//
// Created by Nikolay Kanchevski on 21.11.23.
//

#pragma once

#include "RenderingResource.h"

namespace Sierra
{

    struct DeviceCreateInfo
    {

    };


    class SIERRA_API Device : public virtual RenderingResource
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual const char* GetName() const = 0;

    protected:
        explicit Device(const DeviceCreateInfo &createInfo);

    };

}
