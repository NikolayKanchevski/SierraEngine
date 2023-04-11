//
// Created by Nikolay Kanchevski on 12.02.23.
//

#pragma once

#ifdef linux
    #undef linux
#endif

namespace Sierra::Engine::Classes
{
    class SystemInformation
    {
    private:
        struct CPU
        {
            struct PhysicalInformation
            {
                uint physicalCoreCount;
                uint logicalCoreCount;
                uint threadsPerCore;
                double frequency;
            };

            enum EndiannessType
            {
                Little = (int) iware::cpu::endianness_t::little,
                Big = (int) iware::cpu::endianness_t::big,
            };

            struct Endianness
            {
                EndiannessType type;
                std::string typeString;
            };

            enum ArchitectureType
            {
                x64 = (int) iware::cpu::architecture_t::x64,
                x86 = (int) iware::cpu::architecture_t::x86,
                ARM = (int) iware::cpu::architecture_t::arm,
                Itanium = (int) iware::cpu::architecture_t::itanium,
                Unknown = (int) iware::cpu::architecture_t::unknown
            };

            struct Architecture
            {
                ArchitectureType type;
                std::string typeString;
            };

            std::string name;

            Architecture architecture;
            Endianness endianness;
            PhysicalInformation physicalInformation;
        };

        struct GPU
        {
            enum VendorType
            {
                Intel = (int) iware::gpu::vendor_t::intel,
                AMD = (int) iware::gpu::vendor_t::amd,
                Nvidia = (int) iware::gpu::vendor_t::nvidia,
                Microsoft = (int) iware::gpu::vendor_t::microsoft,
                Qualcomm = (int) iware::gpu::vendor_t::qualcomm,
                Apple = (int) iware::gpu::vendor_t::apple,
                Unknown = (int) iware::gpu::vendor_t::unknown
            };

            struct PhysicalInformation
            {
                uint64 totalMemory;
                double maxFrequency;
            };

            struct Vendor
            {
                std::string name;

                VendorType type;
            };

        public:
            std::string name;

            Vendor vendor;
            PhysicalInformation physicalInformation;

            uint64 GetUsedVideoMemory() const;
        };

        struct Memory
        {
            uint64 totalPhysicalMemory;
            uint64 totalVirtualMemory;

            inline uint64 GetAvailablePhysicalMemory() const { return iware::system::memory().physical_available; }
            inline uint64 GetAvailableVirtualMemory() const { return iware::system::memory().virtual_available; }
        };

        struct Kernel
        {
            enum Type
            {
                WindowsNT = (int) iware::system::kernel_t::windows_nt,
                Linux = (int) iware::system::kernel_t::linux,
                Darwin = (int) iware::system::kernel_t::darwin,
                Unknown = (int) iware::system::kernel_t::unknown
            };

            struct Version
            {
                uint major;
                uint minor;
                uint patch;
            };

        public:
            std::string name;

            Type type;
            Version version;
            uint buildNumber;
        };

        struct OS
        {
            enum Type
            {
                Windows,
                Linux,
                MacOS,
                MacCatalyst,
                UNIX,
                POSIX,
                Android,
                iOS,
                Unknown
            };

            struct Version
            {
                uint major;
                uint minor;
                uint patch;
            };

        public:
            std::string name;

            Type type;
            Version version;
            uint buildNumber;
        };

        struct Display
        {
            Vector2 resolution;
            uint DPI;
            uint bitsPerPixel;
            float refreshRate;
        };

        struct ExternalDevicesInformation
        {
            uint connectedMicesCount;
            uint connectedKeyboardsCount;
            uint unknownConnectedDevicesCount;
        };

    public:
        /* --- POLLING METHODS --- */
        static void Start();
        static void Shutdown();

        /* --- GETTER METHODS --- */
        [[nodiscard]] static inline CPU& GetCPU() { return cpu; };

        [[nodiscard]] static inline uint GetGPUCount() { return gpuCount; };
        [[nodiscard]] static inline GPU& GetGPU(const uint index = 0) { return gpus[index]; }

        [[nodiscard]] static inline Memory& GetMemory() { return memory; };
        [[nodiscard]] static inline Kernel& GetKernel() { return kernel; };
        [[nodiscard]] static inline OS& GetOperatingSystem() { return os; };

        [[nodiscard]] static inline uint GetDisplayCount() { return displayCount; };
        [[nodiscard]] static inline Display& GetDisplay(const uint index = 0) { return displays[index]; };

        [[nodiscard]] static ExternalDevicesInformation& GetExternalDeviceInformation() { return externalDevicesInformation; }

    private:
        static inline CPU cpu;
        static inline uint gpuCount;
        static inline GPU* gpus;
        static inline Memory memory;
        static inline Kernel kernel;
        static inline OS os;
        static inline uint displayCount;
        static inline Display* displays;
        static inline ExternalDevicesInformation externalDevicesInformation;

        static void GetArchitectureName(CPU::ArchitectureType architectureType, std::string &output);
        static void GetEndiannesName(CPU::EndiannessType endiannessType, std::string &output);
        static void GetVendorName(GPU::VendorType vendorType, std::string &output);
        static void GetKernelName(Kernel::Type kernelType, std::string &output);
    };
}
