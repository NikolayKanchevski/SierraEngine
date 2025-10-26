//
// Created by Nikolay Kanchevski on 8.10.25.
//

#pragma once

namespace SierraEngine
{

    struct ProjectCreateInfo
    {
        const Sierra::FileManager& fileManager;
        const std::filesystem::path& projectDirectoryPath = { };
    };

    class Project final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Project(const ProjectCreateInfo& createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] const std::filesystem::path& GetDirectoryPath() const noexcept { return directoryPath; }
        [[nodiscard]] std::filesystem::path GetAssetPath() const noexcept { return directoryPath / "Assets"; }
        [[nodiscard]] std::filesystem::path GetDataPath() const noexcept { return directoryPath / "Data"; }


        /* --- COPY SEMANTICS --- */
        Project(const Project&) = delete;
        Project& operator=(const Project&) = delete;

        /* --- MOVE SEMANTICS --- */
        Project(Project&&) noexcept = default;
        Project& operator=(Project&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~Project() noexcept = default;
    private:
        std::filesystem::path directoryPath = { };

    };

}