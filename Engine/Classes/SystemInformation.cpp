//
// Created by Nikolay Kanchevski on 12.02.23.
//

#include "SystemInformation.h"

namespace Sierra::Engine::Classes
{

    /* --- POLLING METHODS --- */

    void SystemInformation::Start()
    {
        // CPU
        {
            cpu = CPU{};

            cpu.name = iware::cpu::model_name().c_str();

            cpu.architecture = CPU::Architecture{};
            cpu.architecture.type = static_cast<CPU::ArchitectureType>(iware::cpu::architecture());
            GetArchitectureName(cpu.architecture.type, cpu.architecture.typeString);

            cpu.endianness = CPU::Endianness{};
            cpu.endianness.type = static_cast<CPU::EndiannessType>(iware::cpu::endianness());
            GetEndiannesName(cpu.endianness.type, cpu.endianness.typeString);

            auto iwareCPU = iware::cpu::quantities();
            cpu.physicalInformation = CPU::PhysicalInformation{};
            cpu.physicalInformation.physicalCoreCount = iwareCPU.physical;
            cpu.physicalInformation.logicalCoreCount = iwareCPU.logical;
            cpu.physicalInformation.threadsPerCore = cpu.physicalInformation.logicalCoreCount / cpu.physicalInformation.physicalCoreCount;
            cpu.physicalInformation.frequency = iware::cpu::frequency();
        }

        // GPU
        {
            auto iwareGPUs = iware::gpu::device_properties();

            gpuCount = iwareGPUs.size();
            gpus = new GPU[gpuCount];

            for (uint i = gpuCount; i--;)
            {
                auto gpu = &gpus[i];
                auto gpuQuantity = &iwareGPUs[i];

                gpu->name = gpuQuantity->name.c_str();

                gpu->vendor = GPU::Vendor{};
                gpu->vendor.type = static_cast<GPU::VendorType>(gpuQuantity->vendor);
                GetVendorName(gpu->vendor.type, gpu->vendor.name);

                gpu->physicalInformation = GPU::PhysicalInformation{};
                gpu->physicalInformation.totalMemory = gpuQuantity->memory_size;
                gpu->physicalInformation.maxFrequency = gpuQuantity->max_frequency;
            }
        }

        // Memory
        {
            auto iwareMemory = iware::system::memory();

            memory = Memory{};
            memory.totalPhysicalMemory = iwareMemory.physical_total;
            memory.totalVirtualMemory = iwareMemory.virtual_total;
        }

        // Kernel
        {
            auto iwareKernel = iware::system::kernel_info();

            kernel = Kernel{};
            kernel.type = static_cast<Kernel::Type>(iwareKernel.variant);
            GetKernelName(kernel.type, kernel.name);

            kernel.version = Kernel::Version{};
            kernel.version.major = iwareKernel.major;
            kernel.version.minor = iwareKernel.minor;
            kernel.version.patch = iwareKernel.patch;

            kernel.buildNumber = iwareKernel.build_number;
        }

        // OS
        {
            auto iwareOS = iware::system::OS_info();

            os = OS{};

            os.version = OS::Version{};
            os.version.major = iwareOS.major;
            os.version.minor = iwareOS.minor;
            os.version.patch = iwareOS.patch;

            os.buildNumber = iwareOS.build_number;

            #if _WIN32
                os.type = OS::Type::Windows;
                os.name = "Windows";
            #elif __APPLE__
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
            #elif __linux
                os.type = OS::Type::Linux;
                os.name = "Linux";
            #elif __unix
                os.type = OS::Type::UNIX;
                os.name = "UNIX";
            #elif __posix
                os.type = OS::Type::POSIX;
                os.name = "POSIX";
            #elif __ANDROID__
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

            for (uint i = displayCount; i--;)
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
            externalDevicesInformation = ExternalDevicesInformation{};
            externalDevicesInformation.connectedMicesCount = iware::system::mouse_amount();
            externalDevicesInformation.connectedKeyboardsCount = iware::system::keyboard_amount();
            externalDevicesInformation.unknownDevicesConnectedCount = iware::system::other_HID_amount();
        }

    }

    void SystemInformation::Shutdown()
    {
        delete[] gpus;
        delete[] displays;
    }

    /* --- PRIVATE METHODS --- */

    void SystemInformation::GetEndiannesName(const CPU::EndiannessType endiannessType, std::string &output)
    {
        switch(endiannessType)
        {
            case CPU::EndiannessType::Little:
                output = "Little-Endian";
                break;
            case CPU::EndiannessType::Big:
                output = "Big-Endian";
                break;
            default:
                output = "Unknown";
                break;
        }
    }

    void SystemInformation::GetArchitectureName(const CPU::ArchitectureType architectureType, std::string &output)
    {
        switch(architectureType)
        {
            case CPU::ArchitectureType::x64:
                output = "x64";
                break;
            case CPU::ArchitectureType::x86:
                output = "x86";
                break;
            case CPU::ArchitectureType::ARM:
                output = "ARM";
                break;
            case CPU::ArchitectureType::Itanium:
                output = "Itanium";
                break;
            default:
                output = "Unknown";
                break;
        }
    }

    void SystemInformation::GetVendorName(const GPU::VendorType vendorType, std::string &output)
    {
        switch (vendorType)
        {
            case GPU::VendorType::Intel:
                output = "Intel";
                break;
            case GPU::VendorType::AMD:
                output = "AMD";
                break;
            case GPU::VendorType::Nvidia:
                output = "NVidia";
                break;
            case GPU::VendorType::Microsoft:
                output = "Microsoft";
                break;
            case GPU::VendorType::Qualcomm:
                output = "Qualcomm";
                break;
            case GPU::VendorType::Apple:
                output = "Apple";
                break;
            default:
                output = "Unknown";
                break;
        }
    }

    void SystemInformation::GetKernelName(const Kernel::Type kernelType, std::string &output)
    {
        switch(kernelType) 
        {
            case Kernel::Type::WindowsNT:
                output = "Windows NT";
                break;
            case Kernel::Type::Linux:
                output = "Linux";
                break;
            case Kernel::Type::Darwin:
                output = "Darwin";
                break;
            default:
                output = "Unknown";
                break;
        }
    }
    
}

/* --- SETTER METHODS --- */

/* --- GETTER METHODS --- */

/* --- DESTRUCTOR --- */
