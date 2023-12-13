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

        /* --- OPERATORS --- */
        PlatformInstance(const PlatformInstance&) = delete;
        PlatformInstance& operator=(const PlatformInstance&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~PlatformInstance() = default;

    protected:
        explicit PlatformInstance(const PlatformInstanceCreateInfo &createInfo);

    private:
        friend class Application;
        static std::unique_ptr<PlatformInstance> Load(const PlatformInstanceCreateInfo &createInfo);

        virtual void RunApplication(const PlatformApplicationRunInfo &runInfo);

    };

}
