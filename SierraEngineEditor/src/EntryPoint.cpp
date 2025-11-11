//
// Created by Nikolay Kanchevski on 13.05.24.
//

#define REQUIRED_ARGS \
    REQUIRED_STRING_ARG(projectDirectoryPath, "project_directory_path", "Directory where project is stored")

#include <easyargs.h>
#include "Core/EditorApplication.h"

using namespace SierraEngine;
Sierra::Application* Sierra::CreateApplication(const int argc, char* argv[])
{
    args_t args = make_default_args();
    if (!parse_args(argc, argv, &args))
    {
        print_help(argv[0]);
        throw std::invalid_argument("Failed to parse command-line arguments.");
    }

    const std::filesystem::path projectDirectoryPath = std::filesystem::path(args.projectDirectoryPath);

    const EditorApplicationCreateInfo createInfo
    {
        .projectDirectoryPath = projectDirectoryPath,
        .settings = {
            .renderingBackendType = RenderingBackendType::Best
        }
    };

    return new EditorApplication(createInfo);
}
