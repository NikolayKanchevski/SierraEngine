//
// Created by Nikolay Kanchevski on 2.01.24.
//

#include "NSFilePaths.h"

#include <Foundation/Foundation.h>

namespace Sierra
{

    /* --- GETTER METHODS --- */

    std::filesystem::path NSFilePaths::GetApplicationDirectoryPath()
    {
        return std::filesystem::path([[[NSBundle mainBundle] bundlePath] cStringUsingEncoding: NSASCIIStringEncoding]);
    }

    std::filesystem::path NSFilePaths::GetUserDirectoryPath()
    {
        return std::filesystem::path([[[NSFileManager defaultManager] URLsForDirectory: NSUserDirectory inDomains: NSUserDomainMask].firstObject.path cStringUsingEncoding: NSASCIIStringEncoding]);
    }

    std::filesystem::path NSFilePaths::GetCachesDirectoryPath()
    {
        return std::filesystem::path([[[NSFileManager defaultManager] URLsForDirectory: NSCachesDirectory inDomains: NSUserDomainMask].firstObject.path cStringUsingEncoding: NSASCIIStringEncoding]);
    }

    std::filesystem::path NSFilePaths::GetTemporaryDirectoryPath()
    {
        return std::filesystem::path([NSTemporaryDirectory() cStringUsingEncoding: NSASCIIStringEncoding]);
    }

}
