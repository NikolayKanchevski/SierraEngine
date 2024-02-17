//
// Created by Nikolay Kanchevski on 30.07.23.
//

#pragma once

namespace Sierra
{

    enum class VersionState : uint8
    {
        Experimental,
        Alpha,
        Beta,
        Stable
    };

    struct VersionCreateInfo
    {
        uint16 major = 0;
        uint16 minor = 0;
        uint16 patch = 0;
        VersionState state = VersionState::Experimental;
    };

    class SIERRA_API Version final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Version(const VersionCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint16 GetMajor() const { return MAJOR; }
        [[nodiscard]] inline uint16 GetMinor() const { return MINOR; }
        [[nodiscard]] inline uint16 GetPatch() const { return PATCH; }
        [[nodiscard]] inline VersionState GetState() const { return STATE; }
        [[nodiscard]] inline std::string ToString() const { return std::to_string(MAJOR) + "." + std::to_string(MINOR) + "." + std::to_string(PATCH); }

        /* --- OPERATORS --- */
        [[nodiscard]] inline bool operator <(const Version &other) const { return MAJOR < other.MAJOR || MINOR < other.MINOR || PATCH < other.PATCH; }
        [[nodiscard]] inline bool operator >(const Version &other) const { return MAJOR > other.MAJOR || MINOR > other.MINOR || PATCH > other.PATCH; }
        [[nodiscard]] inline bool operator <=(const Version &other) const { return MAJOR <= other.MAJOR || MINOR <= other.MINOR || PATCH <= other.PATCH; }
        [[nodiscard]] inline bool operator >=(const Version &other) const { return MAJOR >= other.MAJOR || MINOR >= other.MINOR || PATCH >= other.PATCH; }
		[[nodiscard]] inline bool operator ==(const Version &other) const { return MAJOR == other.MAJOR && MINOR == other.MINOR && PATCH == other.PATCH; }
		[[nodiscard]] inline bool operator !=(const Version &other) const { return !(*this == other); }

    private:
        const uint16 MAJOR;
        const uint16 MINOR;
        const uint16 PATCH;
        const VersionState STATE;

    };

}