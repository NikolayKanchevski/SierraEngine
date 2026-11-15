//
// Created by Nikolay Kanchevski on 8.11.2023.
//

#pragma once

#include <spdlog/spdlog.h>

namespace Sierra
{

    class SIERRA_API Logger final
    {
    public:
        /* --- GETTER METHODS --- */
        #if defined(SR_LIBRARY_IMPLEMENTATION)
            [[nodiscard]] static spdlog::logger& GetCoreLogger() noexcept;
        #endif
        [[nodiscard]] static spdlog::logger& GetApplicationLogger() noexcept;

        /* --- COPY SEMANTICS --- */
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;

        /* --- MOVE SEMANTICS --- */
        Logger(Logger&&) = delete;
        Logger& operator=(Logger&&) = delete;

        /* --- DESTRUCTOR --- */
        ~Logger() noexcept = default;

    private:
        friend class Application;
        static void Initialize(std::string_view applicationName);

    };

}