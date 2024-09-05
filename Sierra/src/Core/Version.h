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
        explicit Version(const VersionCreateInfo& createInfo) : major(createInfo.major), minor(createInfo.minor), patch(createInfo.patch) { }

        /* --- GETTER METHODS --- */
        [[nodiscard]] uint8 GetMajor() const { return major; }
        [[nodiscard]] uint8 GetMinor() const { return minor; }
        [[nodiscard]] uint8 GetPatch() const { return patch; }

        /* --- COPY SEMANTICS --- */
        Version(const Version&) = default;
        Version& operator=(const Version&) = default;

        /* --- MOVE SEMANTICS --- */
        Version(Version&&) = default;
        Version& operator=(Version&&) = default;

        /* --- OPERATORS --- */
        [[nodiscard]] bool operator<(const Version other) const { return major < other.major || minor < other.minor || patch < other.patch; }
        [[nodiscard]] bool operator>(const Version other) const { return major > other.major || minor > other.minor || patch > other.patch; }
        [[nodiscard]] bool operator<=(const Version other) const { return major <= other.major || minor <= other.minor || patch <= other.patch; }
        [[nodiscard]] bool operator>=(const Version other) const { return major >= other.major || minor >= other.minor || patch >= other.patch; }
		[[nodiscard]] bool operator==(const Version other) const { return major == other.major && minor == other.minor && patch == other.patch; }
		[[nodiscard]] bool operator!=(const Version other) const { return !(*this == other); }

        /* --- DESTRUCTOR --- */
        ~Version() = default;

    private:
        uint8 major = 0;
        uint8 minor = 0;
        uint8 patch = 0;

    };

}