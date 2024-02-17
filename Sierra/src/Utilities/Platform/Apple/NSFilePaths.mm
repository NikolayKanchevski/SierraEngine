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
        return { [[[NSBundle mainBundle] bundlePath] cStringUsingEncoding: NSASCIIStringEncoding] };
    }

    std::filesystem::path NSFilePaths::GetUserDirectoryPath()
    {
        return { [[[NSFileManager defaultManager] URLsForDirectory: NSUserDirectory inDomains: NSUserDomainMask].firstObject.path cStringUsingEncoding: NSASCIIStringEncoding] };
    }

    std::filesystem::path NSFilePaths::GetCachesDirectoryPath()
    {
        return { [[[NSFileManager defaultManager] URLsForDirectory: NSCachesDirectory inDomains: NSUserDomainMask].firstObject.path cStringUsingEncoding: NSASCIIStringEncoding] };
    }

    std::filesystem::path NSFilePaths::GetTemporaryDirectoryPath()
    {
        return { [NSTemporaryDirectory() cStringUsingEncoding: NSASCIIStringEncoding] };
    }

}
