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
        explicit Version(const VersionCreateInfo& createInfo) noexcept;

        /* --- GETTER METHODS --- */
        [[nodiscard]] uint8 GetMajor() const noexcept { return major; }
        [[nodiscard]] uint8 GetMinor() const noexcept { return minor; }
        [[nodiscard]] uint8 GetPatch() const noexcept { return patch; }

        /* --- COPY SEMANTICS --- */
        Version(const Version&) noexcept = default;
        Version& operator=(const Version&) noexcept = default;

        /* --- MOVE SEMANTICS --- */
        Version(Version&&) noexcept = default;
        Version& operator=(Version&&) noexcept = default;

        /* --- OPERATORS --- */
        [[nodiscard]] bool operator<(const Version other) const noexcept { return major < other.major || minor < other.minor || patch < other.patch; }
        [[nodiscard]] bool operator>(const Version other) const noexcept { return major > other.major || minor > other.minor || patch > other.patch; }
        [[nodiscard]] bool operator<=(const Version other) const noexcept { return major <= other.major || minor <= other.minor || patch <= other.patch; }
        [[nodiscard]] bool operator>=(const Version other) const noexcept { return major >= other.major || minor >= other.minor || patch >= other.patch; }
		[[nodiscard]] bool operator==(const Version other) const noexcept { return major == other.major && minor == other.minor && patch == other.patch; }
		[[nodiscard]] bool operator!=(const Version other) const noexcept { return !(*this == other); }

        /* --- DESTRUCTOR --- */
        ~Version() noexcept = default;

    private:
        uint8 major = 0;
        uint8 minor = 0;
        uint8 patch = 0;

    };

}