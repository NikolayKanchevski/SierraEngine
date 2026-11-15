//
// Created by Nikolay Kanchevski on 15.11.26.
//

#pragma once

namespace Sierra
{

    class SIERRA_API ScopeProfiler final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit ScopeProfiler(std::string_view scopeName);

        /* --- COPY SEMANTICS --- */
        ScopeProfiler(const ScopeProfiler&) = delete;
        ScopeProfiler& operator=(const ScopeProfiler&) = delete;

        /* --- MOVE SEMANTICS --- */
        ScopeProfiler(ScopeProfiler&&) = delete;
        ScopeProfiler& operator=(ScopeProfiler&&) = delete;

        /* --- DESTRUCTOR --- */
        ~ScopeProfiler() noexcept;

    private:
        std::string scopeName = { };
        const std::chrono::time_point<std::chrono::high_resolution_clock> startTime = { };

    };

}