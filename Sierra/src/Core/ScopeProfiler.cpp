//
// Created by Nikolay Kanchevski on 9.1.23.
//

#include "ScopeProfiler.h"

#include "Logger.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    ScopeProfiler::ScopeProfiler(const std::string &scopeSignature)
        : startTime(std::chrono::high_resolution_clock::now())
    {
        // Format the scopeSignature name if it is a function
        if (size leftBracketPosition = scopeSignature.find('('); leftBracketPosition != std::string::npos)
        {
            // Remove method arguments if there are any
            std::string fullMethodName = scopeSignature.substr(0, leftBracketPosition);
            fullMethodName = std::regex_replace(fullMethodName, std::regex("\\ "), "> ");

            size returnTypeSpacePosition = fullMethodName.rfind("> ");
            fullMethodName = "::" + fullMethodName.substr(returnTypeSpacePosition + 2, fullMethodName.size() - 1 + 2);

             // Remove template arguments if there are any
            if (size leftAngleBracketPosition = fullMethodName.find('<'); leftAngleBracketPosition != std::string::npos)
            {
                size rightAngleBracketPosition = fullMethodName.rfind('>');
                fullMethodName = fullMethodName.substr(0, leftAngleBracketPosition) + fullMethodName.substr(rightAngleBracketPosition, fullMethodName.size() - rightAngleBracketPosition - 1);
            }

            // Remove all namespaces and class names
            size lastColonPosition = fullMethodName.rfind(':');
            std::string methodName = fullMethodName.substr(lastColonPosition + 1, fullMethodName.size() - lastColonPosition - 1);

            // Get last namespace or class name, depending on where the method is defined
            std::string lastNamespace = fullMethodName.substr(0, fullMethodName.rfind(':') - 1);

            lastColonPosition = lastNamespace.rfind(':');
            lastNamespace = lastNamespace.substr(lastColonPosition + 1, lastNamespace.size() - lastColonPosition - 1);
            scopeName = lastNamespace + "::" + methodName + "()";
        }
        else
        {
            scopeName = "Anonymous Scope";
        }
    }

    /* --- DESTRUCTOR --- */

    ScopeProfiler::~ScopeProfiler()
    {
        const auto endTime = std::chrono::high_resolution_clock::now();
        const uint64 duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        SR_INFO("{0} took {1}ms", scopeName, duration);
    }

}