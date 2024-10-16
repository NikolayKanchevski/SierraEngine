//
// Created by Nikolay Kanchevski on 30.07.23.
//

#pragma once

namespace Sierra
{

    struct VersionCreateInfo
    {
        uint32 major = 0;
        uint32 minor = 0;
        uint32 patch = 0;
    };

    class SIERRA_API Version final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Version(VersionCreateInfo createInfo) noexcept;

        /* --- GETTER METHODS --- */
        [[nodiscard]] uint32 GetMajor() const noexcept { return major; }
        [[nodiscard]] uint32 GetMinor() const noexcept { return minor; }
        [[nodiscard]] uint32 GetPatch() const noexcept { return patch; }

        /* --- COPY SEMANTICS --- */
        Version(const Version&) noexcept = default;
        Version& operator=(const Version&) noexcept = default;

        /* --- MOVE SEMANTICS --- */
        Version(Version&&) noexcept = default;
        Version& operator=(Version&&) noexcept = default;

        /* --- OPERATORS --- */
        [[nodiscard]] bool operator<(Version other) const noexcept { return major < other.major || minor < other.minor || patch < other.patch; }
        [[nodiscard]] bool operator>(Version other) const noexcept { return major > other.major || minor > other.minor || patch > other.patch; }
        [[nodiscard]] bool operator<=(Version other) const noexcept { return major <= other.major || minor <= other.minor || patch <= other.patch; }
        [[nodiscard]] bool operator>=(Version other) const noexcept { return major >= other.major || minor >= other.minor || patch >= other.patch; }
		[[nodiscard]] bool operator==(Version other) const noexcept { return major == other.major && minor == other.minor && patch == other.patch; }
		[[nodiscard]] bool operator!=(Version other) const noexcept { return !(*this == other); }

        /* --- DESTRUCTOR --- */
        ~Version() noexcept = default;

    private:
        uint32 major = 0;
        uint32 minor = 0;
        uint32 patch = 0;

    };

}