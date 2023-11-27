//
// Created by Nikolay Kanchevski on 10.09.23.
//

#pragma once

namespace Sierra
{

    BETTER_ENUM(
        PlatformType, uint8,
        Windows,
        Linux,
        MacOS,
        iOS,
        Android
    );

    struct PlatformInstanceCreateInfo
    {

    };

    struct PlatformApplicationRunInfo
    {
        std::function<void()> OnStart = [] {  };
        std::function<bool()> OnUpdate = [] { return false; };
        std::function<void()> OnEnd = [] {  };
    };

    class SIERRA_API PlatformInstance
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual PlatformType GetType() const = 0;

        /* --- DESTRUCTOR --- */
        virtual ~PlatformInstance() = default;

        /* --- OPERATORS --- */
        PlatformInstance(const PlatformInstance&) = delete;
        PlatformInstance& operator=(const PlatformInstance&) = delete;

    protected:
        explicit PlatformInstance(const PlatformInstanceCreateInfo &createInfo);

    private:
        friend class Application;
        static std::unique_ptr<PlatformInstance> Load(const PlatformInstanceCreateInfo &createInfo);

        virtual void RunApplication(const PlatformApplicationRunInfo &runInfo);

    };

}
