//
// Created by Nikolay Kanchevski on 12.02.23.
//

#pragma once

#ifdef linux
    #undef linux
#endif

namespace Sierra::Engine
{
    class SystemInformation
    {
    private:
        struct CPU
        {
            struct PhysicalInformation
            {
                uint32 physicalCoreCount;
                uint32 logicalCoreCount;
                uint32 threadsPerCore;
                uint64 frequency;
            };

            enum class EndiannessType
            {
                Little = (int) iware::cpu::endianness_t::little,
                Big = (int) iware::cpu::endianness_t::big,
            };

            enum class ArchitectureType
            {
                x64 = (int) iware::cpu::architecture_t::x64,
                x86 = (int) iware::cpu::architecture_t::x86,
                ARM = (int) iware::cpu::architecture_t::arm,
                Itanium = (int) iware::cpu::architecture_t::itanium,
                Unknown = (int) iware::cpu::architecture_t::unknown
            };

            String name;
            EndiannessType endianness;
            ArchitectureType architecture;
            PhysicalInformation physicalInformation;

            [[nodiscard]] inline const char* GetEndiannessString()
            {
                switch (endianness)
                {
                    case EndiannessType::Little:
                        return "Little";
                    case EndiannessType::Big:
                        return "Big";
                }
            }

            [[nodiscard]] inline const char* GetArchitectureString()
            {
                switch (architecture)
                {
                    case ArchitectureType::x64:
                        return "x64";
                    case ArchitectureType::x86:
                        return "x86";
                    case ArchitectureType::ARM:
                        return "ARM";
                    case ArchitectureType::Itanium:
                        return "Itanium";
                    case ArchitectureType::Unknown:
                        return "Unknown";
                }
            }
        };

        struct GPU
        {
            enum class VendorType
            {
                Intel,
                AMD,
                Nvidia,
                Microsoft,
                Qualcomm,
                Apple,
                Unknown
            };

            struct PhysicalInformation
            {
                uint64 totalMemory;
                [[nodiscard]] uint64 GetUsedVideoMemory() const;
            };

        public:
            String name;
            VendorType vendor;
            PhysicalInformation physicalInformation;

            [[nodiscard]] inline const char* GetVendorString()
            {
                switch (vendor)
                {
                    case VendorType::Intel:
                        return "Intel";
                    case VendorType::AMD:
                        return "AMD";
                    case VendorType::Nvidia:
                        return "Nvidia";
                    case VendorType::Microsoft:
                        return "Microsoft";
                    case VendorType::Qualcomm:
                        return "Qualcomm";
                    case VendorType::Apple:
                        return "Apple";
                    case VendorType::Unknown:
                        return "Unknown";
                }
            }
        };

        struct Memory
        {
            uint64 totalPhysicalMemory;
            uint64 totalVirtualMemory;

            [[nodiscard]] inline uint64 GetAvailablePhysicalMemory() const { return iware::system::memory().physical_available; }
            [[nodiscard]] inline uint64 GetAvailableVirtualMemory() const { return iware::system::memory().virtual_available; }
        };

        struct Kernel
        {
            enum class Type
            {
                WindowsNT = (int) iware::system::kernel_t::windows_nt,
                Linux = (int) iware::system::kernel_t::linux,
                Darwin = (int) iware::system::kernel_t::darwin,
                Unknown = (int) iware::system::kernel_t::unknown
            };

            struct Version
            {
                uint32 major;
                uint32 minor;
                uint32 patch;
            };

        public:
            Type type;
            Version version;
            uint32 buildNumber;

            [[nodiscard]] inline const char* GetTypeString()
            {
                switch (type)
                {
                    case Type::WindowsNT:
                        return "WindowsNT";
                    case Type::Linux:
                        return "Linux";
                    case Type::Darwin:
                        return "Darwin";
                    case Type::Unknown:
                        return "Unknown";
                }
            }
        };

        struct OS
        {
            enum class Type
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
                uint32 major;
                uint32 minor;
                uint32 patch;
            };

        public:
            const char* name;
            Type type;
            Version version;
            uint32 buildNumber;
        };

        struct Display
        {
            Vector2 resolution = { 0.0f, 0.0f };
            uint32 DPI = 0;
            uint32 bitsPerPixel = 0;
            double refreshRate = 0.0;
        };

        struct ExternalDevicesInformation
        {
            uint32 connectedMiceCount;
            uint32 connectedKeyboardsCount;
            uint32 unknownConnectedDevicesCount;
        };

    public:
        /* --- POLLING METHODS --- */
        static void Initialize();
        static void Shutdown();

        /* --- GETTER METHODS --- */
        [[nodiscard]] static inline CPU& GetCPU() { return cpu; };
        [[nodiscard]] static inline GPU& GetGPU() { return gpu; }
        [[nodiscard]] static inline Memory& GetMemory() { return memory; };
        [[nodiscard]] static inline Kernel& GetKernel() { return kernel; };
        [[nodiscard]] static inline OS& GetOperatingSystem() { return os; };
        [[nodiscard]] static inline uint32 GetDisplayCount() { return displayCount; };
        [[nodiscard]] static inline Display& GetDisplay(const uint32 index = 0) { return displays[index]; };
        [[nodiscard]] static ExternalDevicesInformation& GetExternalDeviceInformation() { return externalDevicesInformation; }

    private:
        static inline CPU cpu;
        static inline GPU gpu;
        static inline Memory memory;
        static inline Kernel kernel;
        static inline OS os;
        static inline uint32 displayCount;
        static inline Display* displays;
        static inline ExternalDevicesInformation externalDevicesInformation;
        static GPU::VendorType GetGPUVendor(int32 vendorID);
    };
}
