//
// Created by Nikolay Kanchevski on 29.07.23.
//

#include "Project.h"

#include "../Classes/File.h"

namespace Sierra::Engine
{
    /* --- PROPERTIES --- */

    struct LaunchConfig
    {
        FilePath projectDirectory = "/";
        FilePath assetDirectory = "/";
        [[nodiscard]] bool IsValid() { return File::DirectoryExists(projectDirectory); }
    };
    LaunchConfig launchConfig{};

    struct ProjectData
    {
        String name = "null";
        FilePath tempDirectory = "/";
    };
    ProjectData projectData{};

    /* --- POLLING METHODS --- */

    void Project::Initialize()
    {
        // Check if config file exists, otherwise create empty one
        String launchConfigFilePath = GetAppDataDirectory() / "launch_config.yaml";
        if (!File::FileExists(launchConfigFilePath))
        {
            File::WriteDataToFile(launchConfigFilePath, "game_name: Sierra Engine Game\nproject_folder_path: null", true, true);
        }

        // Load configuration file and settings
        YAML::Node node = YAML::LoadFile(launchConfigFilePath);
        launchConfig.projectDirectory = node["project_folder_path"].as<String>();
        launchConfig.assetDirectory = launchConfig.projectDirectory / "Assets/";

        // Check launch configuration is valid
        ASSERT_ERROR_IF(!launchConfig.IsValid(), "Project folder directory [{0}] is not specified or is an invalid path in configuration file [{0}]", launchConfig.projectDirectory.c_str(), launchConfigFilePath);

        // Load project configuration
        projectData.name = launchConfig.projectDirectory.parent_path().filename();
        projectData.tempDirectory = File::GetTempFolderPath() / "SierraTemp" / projectData.name;
    }

    /* --- GETTER METHODS --- */

    String Project::GetName()                         { return projectData.name; }
    FilePath Project::GetProjectDirectory()           { return launchConfig.projectDirectory; }
    FilePath Project::GetAssetDirectory()             { return launchConfig.assetDirectory; }
    FilePath Project::GetTempDirectory()              { return projectData.tempDirectory; }
    FilePath Project::GetAppDataDirectory()           { return File::GetAppDataFolderPath() / "Sierra/"; }

}