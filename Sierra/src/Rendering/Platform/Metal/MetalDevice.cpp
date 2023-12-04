//
// Created by Nikolay Kanchevski on 21.11.23.
//

#include "MetalDevice.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MetalDevice::MetalDevice(const MetalDeviceCreateInfo &createInfo)
        : Device(createInfo), device(MTL::CreateSystemDefaultDevice())
    {

    }

    /* --- DESTRUCTOR --- */

    void MetalDevice::Destroy()
    {
        device->release();
    }

}