//
// Created by Nikolay Kanchevski on 15.11.26.
//

#include "ScopeProfiler.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    ScopeProfiler::ScopeProfiler(const std::string_view scopeName)
        : scopeName(scopeName), startTime(std::chrono::high_resolution_clock::now())
    {

    }

    /* --- DESTRUCTOR --- */

    ScopeProfiler::~ScopeProfiler() noexcept
    {
        const auto endTime = std::chrono::high_resolution_clock::now();
        const uint64 duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        SR_INFO("Scope [{0}] took {1}ms to execute", scopeName, duration);
    }

}