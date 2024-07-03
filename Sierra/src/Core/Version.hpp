//
// Created by Nikolay Kanchevski on 30.07.23.
//

#pragma once

namespace Sierra
{

    struct VersionCreateInfo
    {
        uint8 major = 0;
        uint8 minor = 0;
        uint8 patch = 0;
    };

    class SIERRA_API Version final
    {
    public:
        /* --- CONSTRUCTORS --- */
        constexpr explicit Version(const VersionCreateInfo &createInfo) : major(createInfo.major), minor(createInfo.minor), patch(createInfo.patch) { }

        /* --- GETTER METHODS --- */
        [[nodiscard]] uint8 GetMajor() const { return major; }
        [[nodiscard]] uint8 GetMinor() const { return minor; }
        [[nodiscard]] uint8 GetPatch() const { return patch; }

        /* --- OPERATORS --- */
        [[nodiscard]] bool operator<(const Version &other) const { return major < other.major || minor < other.minor || patch < other.patch; }
        [[nodiscard]] bool operator>(const Version &other) const { return major > other.major || minor > other.minor || patch > other.patch; }
        [[nodiscard]] bool operator<=(const Version &other) const { return major <= other.major || minor <= other.minor || patch <= other.patch; }
        [[nodiscard]] bool operator>=(const Version &other) const { return major >= other.major || minor >= other.minor || patch >= other.patch; }
		[[nodiscard]] bool operator==(const Version &other) const { return major == other.major && minor == other.minor && patch == other.patch; }
		[[nodiscard]] bool operator!=(const Version &other) const { return !(*this == other); }

    private:
        const uint8 major = 0;
        const uint8 minor = 0;
        const uint8 patch = 0;

    };

}