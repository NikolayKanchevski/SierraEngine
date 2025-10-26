//
// Created by Nikolay Kanchevski on 8.10.25.
//

#include "Project.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    Project::Project(const ProjectCreateInfo& createInfo)
        : directoryPath(createInfo.projectDirectoryPath)
    {
    }

}