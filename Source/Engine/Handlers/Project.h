//
// Created by Nikolay Kanchevski on 29.07.23.
//

#pragma once

namespace Sierra::Engine::Project
{

    /* --- POLLING METHODS --- */
    void Initialize();

    /* --- GETTER METHODS --- */
    [[nodiscard]] String GetName();
    [[nodiscard]] FilePath GetProjectDirectory();
    [[nodiscard]] FilePath GetAssetDirectory();
    [[nodiscard]] FilePath GetTempDirectory();
    [[nodiscard]] FilePath GetAppDataDirectory();

}