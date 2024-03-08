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
        explicit Version(const VersionCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint8 GetMajor() const { return major; }
        [[nodiscard]] inline uint8 GetMinor() const { return minor; }
        [[nodiscard]] inline uint8 GetPatch() const { return patch; }

        /* --- OPERATORS --- */
        [[nodiscard]] inline bool operator <(const Version &other) const { return major < other.major || minor < other.minor || patch < other.patch; }
        [[nodiscard]] inline bool operator >(const Version &other) const { return major > other.major || minor > other.minor || patch > other.patch; }
        [[nodiscard]] inline bool operator <=(const Version &other) const { return major <= other.major || minor <= other.minor || patch <= other.patch; }
        [[nodiscard]] inline bool operator >=(const Version &other) const { return major >= other.major || minor >= other.minor || patch >= other.patch; }
		[[nodiscard]] inline bool operator ==(const Version &other) const { return major == other.major && minor == other.minor && patch == other.patch; }
		[[nodiscard]] inline bool operator !=(const Version &other) const { return !(*this == other); }

    private:
        uint8 major = 0;
        uint8 minor = 0;
        uint8 patch = 0;

    };

}