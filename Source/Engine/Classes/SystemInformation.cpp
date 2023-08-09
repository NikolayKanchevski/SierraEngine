//
// Created by Nikolay Kanchevski on 12.02.23.
//

#include "SystemInformation.h"

#include "../../Core/Rendering/Bases/VK.h"

namespace Sierra::Engine
{

    /* --- POLLING METHODS --- */

    void SystemInformation::Initialize()
    {
        // CPU
        {
            auto iwareCPU = iware::cpu::quantities();

            cpu = CPU();
            cpu.name = iware::cpu::model_name();
            cpu.architecture = static_cast<CPU::ArchitectureType>(iware::cpu::architecture());
            cpu.endianness = static_cast<CPU::EndiannessType>(iware::cpu::endianness());
            
            cpu.physicalInformation = CPU::PhysicalInformation();
            cpu.physicalInformation.physicalCoreCount = iwareCPU.physical;
            cpu.physicalInformation.logicalCoreCount = iwareCPU.logical;
            cpu.physicalInformation.threadsPerCore = cpu.physicalInformation.logicalCoreCount / cpu.physicalInformation.physicalCoreCount;
            cpu.physicalInformation.frequency = iware::cpu::frequency();
        }

        // GPU
        {
            gpu = GPU();
            gpu.name = Rendering::VK::GetDevice()->GetPhysicalDeviceProperties().deviceName;
            gpu.vendor = GetGPUVendor(Rendering::VK::GetDevice()->GetPhysicalDeviceProperties().vendorID);

            VmaBudget budget;
            vmaGetHeapBudgets(Rendering::VK::GetMemoryAllocator(), &budget);
            gpu.physicalInformation.totalMemory = budget.budget;
        }

        // Memory
        {
            auto iwareMemory = iware::system::memory();

            memory = Memory();
            memory.totalPhysicalMemory = iwareMemory.physical_total;
            memory.totalVirtualMemory = iwareMemory.virtual_total;
        }

        // Kernel
        {
            auto iwareKernel = iware::system::kernel_info();

            kernel = Kernel();
            kernel.type = static_cast<Kernel::Type>(iwareKernel.variant);

            kernel.version = Kernel::Version();
            kernel.version.major = iwareKernel.major;
            kernel.version.minor = iwareKernel.minor;
            kernel.version.patch = iwareKernel.patch;

            kernel.buildNumber = iwareKernel.build_number;
        }

        // OS
        {
            auto iwareOS = iware::system::OS_info();

            os = OS();
            os.version = OS::Version();
            os.version.major = iwareOS.major;
            os.version.minor = iwareOS.minor;
            os.version.patch = iwareOS.patch;

            os.buildNumber = iwareOS.build_number;

            #if PLATFORM_WINDOWS
                os.type = OS::Type::Windows;
                os.name = "Windows";
            #elif PLATFORM_APPLE
                #if TARGET_OS_IPHONE && TARGET_OS_MACCATALYST
                    os.type = OS::Type::MacCatalyst;
                    os.name = "Mac Catalyst";
                #elif TARGET_OS_IPHONE
                    os.type = OS::Type::iOS;
                    os.name = "iOS";
                #else
                    os.type = OS::Type::MacOS;
                    os.name = "MacOS";
                #endif
            #elif PLATFORM_LINUX
                os.type = OS::Type::Linux;
                os.name = "Linux";
            #elif __unix
                os.type = OS::Type::UNIX;
                os.name = "UNIX";
            #elif __posix
                os.type = OS::Type::POSIX;
                os.name = "POSIX";
            #elif PLPLATFORM_ANDROID
                os.type = OS::Type::Android;
                os.name = "Android";
            #else
                os.type = OS::Type::Unknown;
                os.name = "Unknown";
            #endif
        }

        // Displays
        {
            auto iwareDisplays = iware::system::displays();

            displayCount = iwareDisplays.size();
            displays = new Display[displayCount];

            for (uint32 i = displayCount; i--;)
            {
                auto display = &displays[i];
                auto iwareDisplay = &iwareDisplays[i];

                display->resolution = { iwareDisplay->width, iwareDisplay->height };
                display->bitsPerPixel = iwareDisplay->bpp;
                display->DPI = iwareDisplay->dpi;
                display->refreshRate = iwareDisplay->refresh_rate;
            }
        }

        // HID
        {
            externalDevicesInformation = ExternalDevicesInformation();
            externalDevicesInformation.connectedMiceCount = iware::system::mouse_amount();
            externalDevicesInformation.connectedKeyboardsCount = iware::system::keyboard_amount();
            externalDevicesInformation.unknownConnectedDevicesCount = iware::system::other_HID_amount();
        }

    }

    uint64 SystemInformation::GPU::PhysicalInformation::GetUsedVideoMemory() const
    {
        VmaBudget budget;
        vmaGetHeapBudgets(Rendering::VK::GetMemoryAllocator(), &budget);
        return budget.usage;
    }

    void SystemInformation::Shutdown()
    {
        delete[](displays);
    }

    /* --- PRIVATE METHODS --- */

    SystemInformation::GPU::VendorType SystemInformation::GetGPUVendor(const int32 vendorID)
    {
        #if PLATFORM_APPLE
            return vendorID == 0x8086 ? GPU::VendorType::Intel : GPU::VendorType::Apple;
        #endif

        switch (vendorID)
        {
            case 0x8086:
                return GPU::VendorType::Intel;
            case 0x1002:
                return GPU::VendorType::AMD;
            case 0x10DE:
                return GPU::VendorType::Nvidia;
            case 0x1414:
                return GPU::VendorType::Microsoft;
            case 0x5143:
                return GPU::VendorType::Qualcomm;
            default:
                return GPU::VendorType::Unknown;
        }
    }
    
}