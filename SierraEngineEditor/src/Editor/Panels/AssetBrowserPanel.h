//
// Created by Nikolay Kanchevski on 12.11.26.
//

#pragma once

#include "../EditorFrame.h"
#include "../Modal.h"
#include "../../Core/ApplicationFrame.h"

namespace SierraEngine
{

    struct AssetBrowserPanelCreateInfo
    {
        const Sierra::Path& rootDirectoryPath = { };
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
        Sierra::TimePoint timeCreated = { };
        Sierra::TimePoint timeLastModified = { };
    };

    class AssetBrowserPanel final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit AssetBrowserPanel(const ApplicationFrame& applicationFrame, const AssetBrowserPanelCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        void Draw(const ApplicationFrame& applicationFrame, const EditorFrame& editorFrame);

        void Refresh(const ApplicationFrame& applicationFrame);

        bool NavigateBack(const ApplicationFrame& applicationFrame);
        bool NavigateForward(const ApplicationFrame& applicationFrame);
        bool NavigateUp(const ApplicationFrame& applicationFrame);
        bool NavigateTo(const ApplicationFrame& applicationFrame, const Sierra::Path& relativeDirectoryPath);

        bool Select(const ApplicationFrame& applicationFrame, const Sierra::Path& relativePath);
        bool Select(const ApplicationFrame& applicationFrame, std::span<const Sierra::Path> relativePaths);
        bool OpenSelection(const ApplicationFrame& applicationFrame);
        bool DeleteSelection(const ApplicationFrame& applicationFrame);
        void ClearSelection();

        bool OpenNewFolderDialog(const ApplicationFrame& applicationFrame, const EditorFrame& editorFrame);
        bool OpenRenameDialog(const ApplicationFrame& applicationFrame, const EditorFrame& editorFrame);
        bool OpenDeleteDialog(const ApplicationFrame& applicationFrame, const EditorFrame& editorFrame);

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
        void HandleShortcuts(const ApplicationFrame& applicationFrame, const EditorFrame& editorFrame);
        void RefreshEntryVisibility();
        void RefreshEntryOrder();

        Sierra::Path rootDirectoryPath = { };
        Sierra::Path relativeDirectoryPath = { };
        void UpdateRelativeDirectoryPath(const ApplicationFrame& applicationFrame, const Sierra::Path& relativeDirectoryPath);
        void DrawNavigation(const ApplicationFrame& applicationFrame, const EditorFrame& editorFrame);

        uint32 filterIndex = 0;
        uint32 sortIndex = 0;
        bool sortDescending = false;
        std::string searchText = { };
        bool listView = false;
        float32 tileSize = 64.0f;
        bool showExtensions = false;
        bool showHiddenEntries = false;
        bool showCompanionFiles = false;
        bool showDetails = false;
        void DrawToolBar(const ApplicationFrame& applicationFrame);

        bool revealCurrentDirectory = true;
        void DrawTree(const ApplicationFrame& applicationFrame, float32 height);
        void DrawTreeNode(const ApplicationFrame& applicationFrame, const Sierra::Path& relativeDirectoryPath);

        std::vector<AssetBrowserEntry> entries = { };
        std::vector<size> visibleEntryIndices = { };
        std::vector<size> selectedEntryIndices = { };
        void AddEntry(const ApplicationFrame& applicationFrame, const Sierra::Path& relativeEntryPath);
        void AddEntry(AssetBrowserEntry&& entry);
        void DrawEntries(const ApplicationFrame& applicationFrame, Vector2 size);
        void DrawEmptyEntries();
        void DrawGridEntries(const ApplicationFrame& applicationFrame);
        void DrawGridEntry(const ApplicationFrame& applicationFrame, const AssetBrowserEntry& entry, Vector2 size);
        void DrawListEntries(const ApplicationFrame& applicationFrame);
        void DrawListEntry(const ApplicationFrame& applicationFrame, const AssetBrowserEntry& entry);
        void HandleEntryInteraction(const ApplicationFrame& applicationFrame, const AssetBrowserEntry& entry);
        void DrawDetails(float32 height);
        void DrawFolderDetails();
        void DrawFileEntryDetails(const AssetBrowserEntry& entry);

        std::string statusMessage = { };
        Sierra::AlertSeverity statusSeverity = Sierra::AlertSeverity::Info;
        float64 statusMessageTime = 0.0;
        void UpdateStatus(const std::string& message, Sierra::AlertSeverity severity);
        void DrawStatusBar();

        bool openBackgroundContextMenu = false;
        bool openSelectionContextMenu = false;
        void DrawPopups(const ApplicationFrame& applicationFrame, const EditorFrame& editorFrame);
        void DrawBackgroundContextMenu(const ApplicationFrame& applicationFrame, const EditorFrame& editorFrame);
        void DrawSelectionContextMenu(const ApplicationFrame& applicationFrame, const EditorFrame& editorFrame);

        // Folder, which an entry asked to be navigated to, which is only done once all entries have been drawn, as navigating replaces them
        std::optional<Sierra::Path> pendingNavigationPath = std::nullopt;

        std::vector<Sierra::Path> backHistory = { };
        std::vector<Sierra::Path> forwardHistory = { };

        [[nodiscard]] AssetBrowserEntry* GetSelectedEntry() noexcept { return !selectedEntryIndices.empty() ? &entries[selectedEntryIndices.front()] : nullptr; }
    };

}
