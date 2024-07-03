//
// Created by Nikolay Kanchevski on 10.09.23.
//

#pragma once

namespace Sierra
{

    enum class PlatformType : uint8
    {
        Windows,
        Linux,
        macOS,
        Android,
        iOS
    };

    class SIERRA_API PlatformContext
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual PlatformType GetType() const = 0;

        /* --- OPERATORS --- */
        PlatformContext(const PlatformContext&) = delete;
        PlatformContext& operator=(const PlatformContext&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~PlatformContext() = default;

    protected:
        PlatformContext() = default;

    };

}
