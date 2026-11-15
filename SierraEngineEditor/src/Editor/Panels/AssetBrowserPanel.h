//
// Created by Nikolay Kanchevski on 12.11.26.
//

#pragma once

#include "../EditorModal.h"
#include "../../Platform/EditorPlatformContext.h"

namespace SierraEngine
{

    struct AssetBrowserPanelCreateInfo
    {
        const Sierra::FileManager& fileManager;
        const RenderingContext& renderingContext;

        const Sierra::Path& rootDirectoryPath = { };
    };

    struct AssetBrowserDrawInfo
    {
        Sierra::CommandBuffer& commandBuffer;
        const Sierra::InputManager& inputManager;
    };

    enum class AssetBrowserEntryType : uint8
    {
        Unknown,
        Folder,
        Blob,
        Image,
        Texture,
        Material,
        Model
    };

    struct AssetBrowserEntry
    {
        std::string name = { };
        Sierra::Path relativePath = { };
        AssetBrowserEntryType type = AssetBrowserEntryType::Unknown;

        size memorySize = 0;
        Sierra::TimePoint dateLastModified = { };
        std::filesystem::file_time_type lastWriteTime = { };
    };

    class AssetBrowserPanel final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit AssetBrowserPanel(const AssetBrowserPanelCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        void Draw(const AssetBrowserDrawInfo& drawInfo);

        void Refresh();

        bool NavigateBack();
        bool NavigateForward();
        bool NavigateUp();
        bool NavigateTo(const Sierra::Path& relativeDirectoryPath);

        bool Select(const Sierra::Path& relativePath);
        bool Select(std::span<const Sierra::Path> relativePaths);
        bool DeleteSelection();
        void ClearSelection();

        bool OpenNewFolderDialog();
        bool OpenRenameDialog();
        bool OpenDeleteDialog();

        /* --- GETTER METHODS --- */
        [[nodiscard]] bool IsSelected(const Sierra::Path& relativePath) const noexcept;

        [[nodiscard]] bool IsSelectionEmpty() const noexcept { return selectedEntryIndices.empty(); }
        [[nodiscard]] const AssetBrowserEntry* GetSelectedEntry() const noexcept { return !selectedEntryIndices.empty() ? &entries[selectedEntryIndices.front()] : nullptr; }

        /* --- COPY SEMANTICS --- */
        AssetBrowserPanel(const AssetBrowserPanel&) = delete;
        AssetBrowserPanel& operator=(const AssetBrowserPanel&) = delete;

        /* --- MOVE SEMANTICS --- */
        AssetBrowserPanel(AssetBrowserPanel&&) noexcept = default;
        AssetBrowserPanel& operator=(AssetBrowserPanel&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~AssetBrowserPanel() noexcept = default;

    private:
        const Sierra::FileManager* fileManager;
        const RenderingContext* renderingContext;

        void HandleShortcuts(const Sierra::InputManager& inputManager);
        void RefreshEntryVisibility();
        void RefreshEntryOrder();

        Sierra::Path rootDirectoryPath = { };
        Sierra::Path relativeDirectoryPath = { };
        void UpdateRelativeDirectoryPath(const Sierra::Path& relativeDirectoryPath);
        void DrawNavigation();

        uint32 filterIndex = 0;
        uint32 sortIndex = 0;
        bool sortDescending = false;
        std::string searchText = { };
        bool listView = false;
        float32 tileSize = 64.0f;
        bool showExtensions = false;
        bool showHiddenEntries = false;
        bool showCompanionFiles = true;
        bool showDetails = false;
        void DrawToolBar();

        bool revealCurrentDirectory = true;
        void DrawTree(float32 height);
        void DrawTreeNode(const Sierra::Path& relativeDirectoryPath);

        std::vector<AssetBrowserEntry> entries = { };
        std::vector<size> visibleEntryIndices = { };
        std::vector<size> selectedEntryIndices = { };
        void AddEntry(const Sierra::Path& relativeEntryPath);
        void DrawEntries(Vector2 size);
        void DrawEmptyEntries();
        void DrawGridEntries();
        void DrawGridEntry(const AssetBrowserEntry& entry, Vector2 size);
        void DrawListEntries();
        void DrawListEntry(const AssetBrowserEntry& entry);
        void HandleEntryInteraction(const AssetBrowserEntry& entry);
        void DrawDetails(float32 height);
        void DrawFolderDetails();
        void DrawFileEntryDetails(const AssetBrowserEntry& entry);

        std::string statusMessage = { };
        AlertSeverity statusSeverity = AlertSeverity::Info;
        float64 statusMessageTime = 0.0;
        void UpdateStatus(const std::string& message, AlertSeverity severity);
        void DrawStatusBar();

        bool openBackgroundContextMenu = false;
        bool openSelectionContextMenu = false;
        std::unique_ptr<EditorModal> modal = nullptr;
        void DrawPopups();
        void DrawBackgroundContextMenu();
        void DrawSelectionContextMenu();

        std::vector<Sierra::Path> backHistory = { };
        std::vector<Sierra::Path> forwardHistory = { };

        [[nodiscard]] AssetBrowserEntry* GetSelectedEntry() noexcept { return !selectedEntryIndices.empty() ? &entries[selectedEntryIndices.front()] : nullptr; }
    };

}
