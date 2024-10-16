//
// Created by Nikolay Kanchevski on 21.11.23.
//

#include "MetalContext.h"

#include "MetalDevice.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MetalContext::MetalContext(const RenderingContextCreateInfo& createInfo)
        : RenderingContext(createInfo), name(createInfo.name)
    {
        /* === Reference: https://support.apple.com/en-us/102894 === */
        const uint32 systemVersionMajor = NSProcessInfo.processInfo.operatingSystemVersion.majorVersion;
        const uint32 systemVersionMinor = NSProcessInfo.processInfo.operatingSystemVersion.minorVersion;
        #if SR_PLATFORM_macOS
            if (systemVersionMajor >= 13 && systemVersionMinor >= 0)       metalVersion = Version({ 3, 0, 0 });
            else if (systemVersionMajor >= 10 && systemVersionMinor >= 11) metalVersion = Version({ 2, 0, 0 });
            else                                                           metalVersion = Version({ 1, 0, 0 });
        #elif SR_PLATFORM_iOS
            if (systemVersionMajor >= 16) metalVersion = Version({ 3, 0, 0 });
            else                          metalVersion = Version({ 2, 0, 0 });
        #endif
    }

    /* --- POLLING METHODS --- */

    std::unique_ptr<Device> MetalContext::CreateDevice(const DeviceCreateInfo& createInfo) const
    {
        NSArray<id<MTLDevice>>* const devices = MTLCopyAllDevices();

        id<MTLDevice> selectedDevice = nil;
        for (const id<MTLDevice> device : devices)
        {
            if (selectedDevice != nil || device.argumentBuffersSupport != MTLArgumentBuffersTier2)
            {
                [device release];
                continue;
            }

            selectedDevice = device;
        }
        [devices release];

        SR_THROW_IF(selectedDevice == nil, UnsupportedFeatureError(SR_FORMAT("Rendering context [{0}] failed to create device [{1}], as no supported GPU was found on the system", name, createInfo.name)));
        return std::make_unique<MetalDevice>(*this, selectedDevice, createInfo);
    }

}
