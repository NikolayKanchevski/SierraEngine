//
// Created by Nikolay Kanchevski on 8.10.25.
//

#pragma once

namespace SierraEngine
{

    struct ProjectCreateInfo
    {
        const Sierra::Path& directoryPath = { };
    };

    class Project final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Project(const ProjectCreateInfo& createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] const Sierra::Path& GetDirectoryPath() const noexcept { return directoryPath; }
        [[nodiscard]] Sierra::Path GetAssetPath() const noexcept { return directoryPath / "Assets"; }
        [[nodiscard]] Sierra::Path GetDataPath() const noexcept { return directoryPath / "Data"; }

        /* --- COPY SEMANTICS --- */
        Project(const Project&) = delete;
        Project& operator=(const Project&) = delete;

        /* --- MOVE SEMANTICS --- */
        Project(Project&&) noexcept = default;
        Project& operator=(Project&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~Project() noexcept = default;

    private:
        Sierra::Path directoryPath = { };

    };

}