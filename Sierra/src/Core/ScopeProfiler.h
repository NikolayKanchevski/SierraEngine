//
// Created by Nikolay Kanchevski on 9.1.23.
//

#pragma once

namespace Sierra
{

    class SIERRA_API ScopeProfiler final
    {
    public:
        /* --- CONSTRUCTORS --- */
        ScopeProfiler(const std::string &scopeSignature);

        /* --- DESTRUCTOR --- */
        ~ScopeProfiler();

    private:
        std::string scopeName;
        const std::chrono::time_point<std::chrono::high_resolution_clock> startTime;

    };

}

#if SR_ENABLE_LOGGING
    #if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
        #define __FUNC_SIG__ __PRETTY_FUNCTION__

    #elif defined(__DMC__) && (__DMC__ >= 0x810)
        #define __FUNC_SIG__ __PRETTY_FUNCTION__

    #elif (defined(__FUNCSIG__) || (_MSC_VER))
        #define __FUNC_SIG__ __FUNCSIG__

    #elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
        #define __FUNC_SIG__ __FUNCTION__

    #elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
        #define __FUNC_SIG__ __FUNC__

    #elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
        #define __FUNC_SIG__ __func__

    #elif defined(__cplusplus) && (__cplusplus >= 201103)
        #define __FUNC_SIG__ __func__

    #else
        #define __FUNC_SIG__ "Unknown FUNC_SIG!"

    #endif
    
    #define PROFILE_SCOPE() ::Sierra::ScopeProfiler profilerLine##__LINE__(__FUNC_SIG__)
#else
    #define PROFILE_SCOPE()
#endif
