//
// Created by Nikolay Kanchevski on 21.11.23.
//

#pragma once

#include "MetalResource.h"
#include "../../Device.h"

namespace Sierra
{

    struct MetalDeviceCreateInfo final : public DeviceCreateInfo
    {

    };

    class SIERRA_API MetalDevice final : public Device, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit MetalDevice(const MetalDeviceCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline const char* GetName() const override { return device->name()->utf8String(); }

        [[nodiscard]] inline const MTL::Device* GetMetalDevice() const { return device; }

        /* --- DESTRUCTOR --- */
        void Destroy() override;

    private:
        MTL::Device* device;

    };

}
