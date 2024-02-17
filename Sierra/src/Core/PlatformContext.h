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

    struct PlatformContextCreateInfo
    {

    };

    struct PlatformApplicationRunInfo
    {
        std::function<void()> OnStart = [] { };
        std::function<bool()> OnUpdate = [] { return false; };
        std::function<void()> OnEnd = [] { };
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
        explicit PlatformContext(const PlatformContextCreateInfo &createInfo);

    private:
        friend class Application;
        static std::unique_ptr<PlatformContext> Create(const PlatformContextCreateInfo &createInfo);
        virtual void RunApplication(const PlatformApplicationRunInfo &runInfo);

    };

}
