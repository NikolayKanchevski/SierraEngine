//
// Created by Nikolay Kanchevski on 12.11.26.
//

#include "AssetBrowserPanel.h"

#include "../Widgets/ImGuiWidgets.h"

#include "../Wizards/Assets/TextureSerializeWizard.h"
#include "../Wizards/Assets/MaterialSerializeWizard.h"
#include "../Wizards/Assets/ModelSerializeWizard.h"

#include  "../Dialogs/ConfirmationDialog.h"
#include  "../Dialogs/TextDialog.h"

namespace SierraEngine
{

    std::string_view AssetBrowserEntryTypeToString(const AssetBrowserEntryType type)
    {
        switch (type)
        {
            case AssetBrowserEntryType::Unknown:        break;
            case AssetBrowserEntryType::Folder:         return "Folder";
            case AssetBrowserEntryType::Blob:           return "Blob";
            case AssetBrowserEntryType::Image:          return "Image";
            case AssetBrowserEntryType::Texture:        return "Texture";
            case AssetBrowserEntryType::Material:       return "Material";
            case AssetBrowserEntryType::Model:          return "Model";
        }

        return "Unknown";
    }

    namespace
    {
        constexpr std::string_view PANEL_TITLE = "Asset Browser";

        constexpr Sierra::KeyCombination NEW_FOLDER_SHORTCUT = { Sierra::Key::PlatformModifier, Sierra::Key::Shift, Sierra::Key::N };
        constexpr Sierra::KeyCombination REFRESH_SHORTCUT = { Sierra::Key::PlatformModifier, Sierra::Key::R };

        constexpr Sierra::KeyCombination OPEN_SHORTCUT = { Sierra::Key::Enter };
        constexpr Sierra::KeyCombination RENAME_SHORTCUT = { Sierra::Key::F2 };

        #if !SR_PLATFORM_APPLE
            constexpr Sierra::KeyCombination DELETE_SHORTCUT = { Sierra::Key::Delete };
        #else
            constexpr Sierra::KeyCombination DELETE_SHORTCUT = { Sierra::Key::Backspace };
        #endif

        constexpr float32 MIN_TILE_SIZE = 56.0f;
        constexpr float32 MAX_TILE_SIZE = 200.0f;
        constexpr float32 TILE_PADDING = 8.0f;
        constexpr float32 TILE_ROUNDING = 8.0f;

        constexpr float32 TREE_WIDTH = 200.0f;

        constexpr float32 DETAILS_WIDTH = 270.0f;
        constexpr float32 DETAILS_PREVIEW_SIZE = 230.0f;
        constexpr float32 DETAILS_LABEL_WIDTH = 92.0f;

        constexpr float32 LIST_ICON_SIZE = 18.0f;
        constexpr float32 LIST_ROW_HEIGHT = 24.0f;

        constexpr float64 STATUS_MESSAGE_DURATION = 6.0;
        constexpr ImColor INFO_COLOR    = { 0.40f, 0.80f, 0.45f, 1.0f };
        constexpr ImColor WARNING_COLOR = { 0.95f, 0.75f, 0.30f, 1.0f };
        constexpr ImColor ERROR_COLOR   = { 0.95f, 0.35f, 0.35f, 1.0f };

        enum class FilterType : uint8
        {
            None,
            Folders,
            Textures,
            Models,
            Materials,
            Images,
            Other
        };

        constexpr std::array<ImGuiDropdownOption, 7> FILTER_OPTIONS
        {
            ImGuiDropdownOption { .text = "All Items" },
            ImGuiDropdownOption { .text = "Folders" },
            ImGuiDropdownOption { .text = "Textures" },
            ImGuiDropdownOption { .text = "Models" },
            ImGuiDropdownOption { .text = "Materials" },
            ImGuiDropdownOption { .text = "Images" },
            ImGuiDropdownOption { .text = "Other" }
        };

        enum class SortType : uint8
        {
            Name,
            Type,
            Modified,
            Size
        };

        constexpr std::array<ImGuiDropdownOption, 4> SORT_OPTIONS
        {
            ImGuiDropdownOption { .text = "Name" },
            ImGuiDropdownOption { .text = "Type" },
            ImGuiDropdownOption { .text = "Modified" },
            ImGuiDropdownOption { .text = "Size" }
        };

        std::string ToLower(std::string text)
        {
            std::ranges::transform(text, text.begin(), [](const unsigned char c) -> char { return static_cast<char>(std::tolower(c)); });
            return text;
        }

        std::string FormatSize(const size bytes)
        {
            constexpr std::array<std::string_view, 4> UNITS = { "B", "KB", "MB", "GB" };

            size unit = 0;
            double value = static_cast<double>(bytes);

            while (value >= 1024.0 && unit < UNITS.size() - 1)
            {
                value /= 1024.0;
                unit++;
            }

            if (unit == 0) return SR_FORMAT("{0} B", bytes);
            return SR_FORMAT("{0:.1f} {1}", value, UNITS[unit]);
        }

        std::string BuildSubtitle(const AssetBrowserEntry& entry)
        {
            return SR_FORMAT("{0} - {1}", AssetBrowserEntryTypeToString(entry.type), FormatSize(entry.memorySize));
        }

        bool DrawBreadcrumb(const int ID, const std::string_view label, const bool removeLeadingSlash = false)
        {
            if (!removeLeadingSlash)
            {
                ImGui::SameLine(0.0f, 2.0f);
                ImGui::TextDisabled("/");
                ImGui::SameLine(0.0f, 2.0f);
            }

            ImGui::PushID(ID);
            const bool result = ImGui::Button(label.data());
            ImGui::PopID();

            return result;
        };

        std::vector<Sierra::Path> ListChildDirectoryPaths(const Sierra::FileManager& fileManager, const Sierra::Path& directoryPath)
        {
            std::vector<Sierra::Path> directoryPaths = { };
            directoryPaths.reserve(5);

            fileManager.EnumeratePaths(directoryPath, [&fileManager, &directoryPaths](const Sierra::Path& path) -> bool
            {
                if (fileManager.IsDirectory(path))
                    directoryPaths.emplace_back(path);

                return true;
            });

            std::sort(directoryPaths.begin(), directoryPaths.end(), [](const Sierra::Path& left, const Sierra::Path& right) -> bool
            {
                const std::string leftString = ToLower(left.GetString());
                const std::string rightString = ToLower(right.GetString());
                return leftString < rightString;
            });

            return directoryPaths;
        }

        ImU32 WithAlpha(const ImU32 color, const uint32 alpha) noexcept
        {
            return (color & 0x00FFFFFFu) | (alpha << 24);
        }

        ImU32 GetTypeColor(const AssetBrowserEntryType type) noexcept
        {
            switch (type)
            {
                case AssetBrowserEntryType::Folder:     return IM_COL32(232, 183, 84, 255);
                case AssetBrowserEntryType::Texture:    return IM_COL32(86, 164, 235, 255);
                case AssetBrowserEntryType::Model:      return IM_COL32(112, 204, 136, 255);
                case AssetBrowserEntryType::Material:   return IM_COL32(178, 128, 232, 255);
                case AssetBrowserEntryType::Image:      return IM_COL32(234, 142, 92, 255);
                case AssetBrowserEntryType::Blob:       return IM_COL32(150, 152, 164, 255);
                case AssetBrowserEntryType::Unknown:    return IM_COL32(140, 146, 158, 255);
            }

            return IM_COL32(140, 146, 158, 255);
        }

        // TODO: Replace these AI-generated glyphs with a texture map
        void DrawItemGlyph(ImDrawList* drawList, const ImVec2 min, const ImVec2 max, const AssetBrowserEntryType type)
        {
            const float32 extent = max.x - min.x;
            const ImVec2 center = { (min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f };
            const ImU32 color = GetTypeColor(type);

            if (type == AssetBrowserEntryType::Folder)
            {
                drawList->AddRectFilled({ min.x + extent * 0.12f, min.y + extent * 0.18f }, { min.x + extent * 0.46f, min.y + extent * 0.34f }, color, extent * 0.05f, ImDrawFlags_RoundCornersTop);
                drawList->AddRectFilled({ min.x + extent * 0.12f, min.y + extent * 0.28f }, { max.x - extent * 0.12f, max.y - extent * 0.16f }, color, extent * 0.06f);
                drawList->AddRectFilled({ min.x + extent * 0.12f, min.y + extent * 0.40f }, { max.x - extent * 0.12f, max.y - extent * 0.16f }, IM_COL32(255, 255, 255, 36), extent * 0.06f);
                return;
            }

            // Tinted tile with a symbol for the kind of asset
            drawList->AddRectFilled(min, max, WithAlpha(color, 40), extent * 0.1f);
            drawList->AddRect(min, max, WithAlpha(color, 120), extent * 0.1f, 0, 1.0f);

            switch (type)
            {
                case AssetBrowserEntryType::Texture:
                {
                    const float32 cell = extent * 0.2f;
                    for (uint32 row = 0; row < 2; row++)
                    {
                        for (uint32 column = 0; column < 2; column++)
                        {
                            const ImVec2 cellMin = { center.x - cell + static_cast<float32>(column) * cell, center.y - cell + static_cast<float32>(row) * cell };
                            const ImU32 cellColor = (row + column) % 2 == 0 ? color : WithAlpha(color, 110);
                            drawList->AddRectFilled(cellMin, { cellMin.x + cell, cellMin.y + cell }, cellColor);
                        }
                    }
                    break;
                }
                case AssetBrowserEntryType::Model:
                {
                    const ImVec2 corners[6] {
                        { center.x, center.y - extent * 0.30f },
                        { center.x + extent * 0.26f, center.y - extent * 0.15f },
                        { center.x + extent * 0.26f, center.y + extent * 0.15f },
                        { center.x, center.y + extent * 0.30f },
                        { center.x - extent * 0.26f, center.y + extent * 0.15f },
                        { center.x - extent * 0.26f, center.y - extent * 0.15f }
                    };

                    drawList->AddPolyline(corners, 6, color, ImDrawFlags_Closed, 2.0f);
                    drawList->AddLine(center, corners[0], color, 1.5f);
                    drawList->AddLine(center, corners[2], color, 1.5f);
                    drawList->AddLine(center, corners[4], color, 1.5f);
                    break;
                }
                case AssetBrowserEntryType::Material:
                {
                    drawList->AddCircleFilled(center, extent * 0.26f, color, 32);
                    drawList->AddCircleFilled({ center.x - extent * 0.08f, center.y - extent * 0.08f }, extent * 0.07f, IM_COL32(255, 255, 255, 140), 16);
                    break;
                }
                case AssetBrowserEntryType::Image:
                {
                    drawList->AddTriangleFilled({ min.x + extent * 0.2f, max.y - extent * 0.22f }, { center.x - extent * 0.04f, center.y - extent * 0.12f }, { max.x - extent * 0.2f, max.y - extent * 0.22f }, color);
                    drawList->AddCircleFilled({ center.x + extent * 0.18f, center.y - extent * 0.2f }, extent * 0.08f, color, 16);
                    break;
                }
                case AssetBrowserEntryType::Blob:
                case AssetBrowserEntryType::Unknown:
                case AssetBrowserEntryType::Folder:
                {
                    const ImVec2 pageMin = { center.x - extent * 0.2f, center.y - extent * 0.27f };
                    const ImVec2 pageMax = { center.x + extent * 0.2f, center.y + extent * 0.27f };
                    drawList->AddRect(pageMin, pageMax, color, 2.0f, 0, 2.0f);
                    for (uint32 line = 0; line < 3; line++)
                    {
                        const float32 y = pageMin.y + extent * (0.14f + 0.1f * static_cast<float32>(line));
                        drawList->AddLine({ pageMin.x + extent * 0.07f, y }, { pageMax.x - extent * 0.07f, y }, color, 1.5f);
                    }
                    break;
                }
            }
        }

        void DrawDetailRow(const std::string_view label, const std::string& value, const bool scrollable = false)
        {
            ImGui::TextDisabled("%.*s", static_cast<int>(label.size()), label.data());
            ImGui::SameLine(DETAILS_LABEL_WIDTH);
            if (!scrollable)
            {
                ImGui::TextWrapped("%s", value.c_str());
                return;
            }

            const float childHeight = ImGui::GetTextLineHeight() + ImGui::GetStyle().WindowPadding.y * 2.0f + ImGui::GetStyle().ScrollbarSize / 2.0f;
            ImGui::BeginChild("##DetailRowScroll", ImVec2(0.0f, childHeight), false, ImGuiWindowFlags_HorizontalScrollbar);
            ImGui::Text("%s", value.c_str());
            ImGui::EndChild();
        }

        [[nodiscard]] AssetBrowserEntryType ExtensionToEntryType(const std::string_view extension)
        {
            if (extension == AssetSerializeWizard::BLOB_OUTPUT_FILE_EXTENSION) return AssetBrowserEntryType::Blob;

            for (const std::string_view inputExtension : TextureSerializeWizard::INPUT_FILE_EXTENSIONS)
            {
                if (extension == inputExtension) return AssetBrowserEntryType::Image;
            }

            if (extension == TextureSerializeWizard::OUTPUT_FILE_EXTENSION) return AssetBrowserEntryType::Texture;
            if (extension == MaterialSerializeWizard::OUTPUT_FILE_EXTENSION) return AssetBrowserEntryType::Material;
            if (extension == ModelSerializeWizard::OUTPUT_FILE_EXTENSION) return AssetBrowserEntryType::Model;

            return AssetBrowserEntryType::Unknown;
        }
    }

    /* --- CONSTRUCTORS --- */

    AssetBrowserPanel::AssetBrowserPanel(const ApplicationFrame& applicationFrame, const AssetBrowserPanelCreateInfo& createInfo)
        : rootDirectoryPath(createInfo.rootDirectoryPath)
    {
        Refresh(applicationFrame);
    }

    /* --- POLLING METHODS --- */

    void AssetBrowserPanel::Draw(const ApplicationFrame& applicationFrame, const EditorFrame& editorFrame)
    {
        if (ImGui::Begin(PANEL_TITLE.data(), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoNav))
        {
            HandleShortcuts(applicationFrame, editorFrame);

            DrawToolBar(applicationFrame);
            DrawNavigation(applicationFrame, editorFrame);

            ImGui::Separator();

            const ImGuiStyle& style = ImGui::GetStyle();
            const float32 statusBarHeight = ImGui::GetTextLineHeightWithSpacing() + style.ItemSpacing.y;
            const float32 bodyHeight = glm::max(0.0f, ImGui::GetContentRegionAvail().y - statusBarHeight);

            const float32 detailsSpace = showDetails ? style.ItemSpacing.x + DETAILS_WIDTH : 0.0f;
            const float32 entriesWidth = glm::max(0.0f, ImGui::GetContentRegionAvail().x - TREE_WIDTH - style.ItemSpacing.x - detailsSpace);

            DrawTree(applicationFrame, bodyHeight);
            ImGui::SameLine();
            DrawEntries(applicationFrame, { entriesWidth, bodyHeight });

            if (pendingNavigationPath.has_value())
            {
                const Sierra::Path navigationPath = std::move(*pendingNavigationPath);
                pendingNavigationPath.reset();

                NavigateTo(applicationFrame, navigationPath);
            }

            if (showDetails)
            {
                ImGui::SameLine();
                DrawDetails(bodyHeight);
            }

            DrawStatusBar();
            DrawPopups(applicationFrame, editorFrame);
        }
        ImGui::End();
    }

    void AssetBrowserPanel::Refresh(const ApplicationFrame& applicationFrame)
    {
        selectedEntryIndices.clear();
        entries.clear();

        const bool searching = !searchText.empty();
        const std::string query = ToLower(searchText);

        const Sierra::Path absolutePath = rootDirectoryPath / relativeDirectoryPath;
        if (!searching)
        {
            applicationFrame.platformContext.GetFileManager().EnumeratePaths(absolutePath, [this, &applicationFrame](const Sierra::Path& path) -> bool
            {
                const Sierra::Path relativePath = path.GetRelativePath(rootDirectoryPath);
                AddEntry(applicationFrame, relativePath);

                return true;
            });
        }
        else
        {
            applicationFrame.platformContext.GetFileManager().EnumeratePaths(absolutePath, [this, &applicationFrame, &query](const Sierra::Path& searchPath) -> bool
            {
                const Sierra::Path relativePath = searchPath.GetRelativePath(rootDirectoryPath);

                if (relativePath.IsWithinHiddenPath())
                    return true;

                if (ToLower(searchPath.GetFileName()).find(query) == std::string::npos)
                    return false;

                AddEntry(applicationFrame, relativePath);
                return false;
            });
        }

        RefreshEntryVisibility();
    }

    bool AssetBrowserPanel::NavigateBack(const ApplicationFrame& applicationFrame) // TODO: Validate history
    {
        if (backHistory.empty())
            return false;

        forwardHistory.push_back(relativeDirectoryPath);

        const Sierra::Path previousDirectoryPath = backHistory.back();
        backHistory.pop_back();

        UpdateRelativeDirectoryPath(applicationFrame, previousDirectoryPath);
        return true;
    }

    bool AssetBrowserPanel::NavigateForward(const ApplicationFrame& applicationFrame)
    {
        if (forwardHistory.empty())
            return false;

        backHistory.push_back(relativeDirectoryPath);

        const Sierra::Path nextDirectoryPath = forwardHistory.back();
        forwardHistory.pop_back();

        UpdateRelativeDirectoryPath(applicationFrame, nextDirectoryPath);
        return true;
    }

    bool AssetBrowserPanel::NavigateUp(const ApplicationFrame& applicationFrame)
    {
        if (relativeDirectoryPath.IsEmpty())
            return false;

        return NavigateTo(applicationFrame, relativeDirectoryPath.GetParentPath());
    }

    bool AssetBrowserPanel::NavigateTo(const ApplicationFrame& applicationFrame, const Sierra::Path& givenRelativeDirectoryPath)
    {
        if (givenRelativeDirectoryPath == relativeDirectoryPath || !applicationFrame.platformContext.GetFileManager().IsDirectory(rootDirectoryPath / givenRelativeDirectoryPath))
            return false;

        backHistory.push_back(relativeDirectoryPath);
        forwardHistory.clear();

        UpdateRelativeDirectoryPath(applicationFrame, givenRelativeDirectoryPath);
        return true;
    }

    bool AssetBrowserPanel::Select(const ApplicationFrame& applicationFrame, const Sierra::Path& relativePath)
    {
        ClearSelection();
        return Select(applicationFrame, std::span(&relativePath, 1));
    }

    bool AssetBrowserPanel::Select(const ApplicationFrame& applicationFrame, std::span<const Sierra::Path> relativePaths)
    {
        bool success = !relativePaths.empty();

        selectedEntryIndices.reserve(relativePaths.size());
        for (const Sierra::Path& relativePath: relativePaths)
        {
            const Sierra::Path absolutePath = rootDirectoryPath / relativePath;
            if (!applicationFrame.platformContext.GetFileManager().PathExists(absolutePath))
            {
                success = false;
                continue;
            }

            const auto iterator = std::ranges::find_if(entries, [&relativePath](const AssetBrowserEntry& entry) -> bool { return entry.relativePath == relativePath; });
            if (iterator == entries.end())
            {
                success = false;
                continue;
            }

            const size index = std::distance(entries.begin(), iterator);
            selectedEntryIndices.emplace_back(index);
        }

        return success;
    }

    bool AssetBrowserPanel::OpenSelection(const ApplicationFrame& applicationFrame)
    {
        const AssetBrowserEntry* entry = GetSelectedEntry();
        if (entry == nullptr) return false;

        const Sierra::Path absolutePath = rootDirectoryPath / entry->relativePath;
        if (applicationFrame.platformContext.GetFileManager().IsDirectory(absolutePath))
        {
            NavigateTo(applicationFrame, entry->relativePath);
            return true;
        }

        return applicationFrame.platformContext.OpenPath(absolutePath);
    }

    bool AssetBrowserPanel::DeleteSelection(const ApplicationFrame& applicationFrame)
    {
        size deletedCount = 0;
        for (const size entryIndex : selectedEntryIndices)
        {
            const AssetBrowserEntry& entry = entries[entryIndex];

            const Sierra::Path absolutePath = rootDirectoryPath / entry.relativePath;
            static_cast<void>(applicationFrame.platformContext.GetFileManager().DeletePath(absolutePath));

            if (applicationFrame.platformContext.GetFileManager().IsFile(absolutePath))
            {
                const Sierra::Path blobPath = Sierra::Path(absolutePath).ChangeExtension(AssetSerializeWizard::BLOB_OUTPUT_FILE_EXTENSION);
                static_cast<void>(applicationFrame.platformContext.GetFileManager().DeletePath(blobPath));
            }

            deletedCount++;
        }

        bool success = deletedCount == selectedEntryIndices.size();
        const std::string message = SR_FORMAT("Deleted [{0}] of [{1}] item{2}", deletedCount, selectedEntryIndices.size(), deletedCount == 0 ? "" : "s");
        UpdateStatus(message, success ? Sierra::AlertSeverity::Info : Sierra::AlertSeverity::Warning);

        ClearSelection();
        Refresh(applicationFrame);

        return success;
    }

    void AssetBrowserPanel::ClearSelection()
    {
        selectedEntryIndices.clear();
    }

    bool AssetBrowserPanel::OpenNewFolderDialog(const ApplicationFrame& applicationFrame, const EditorFrame& editorFrame)
    {
        const TextDialogCreateInfo createInfo
        {
            .title = "New Folder",
            .text = "Folder name:",
            .placeholder = "Folder",
            .ValidationCallback = [this, &applicationFrame](const std::string_view name) -> std::string
            {
                if (!Sierra::IsPathNameValid(name))
                    return SR_FORMAT("Cannot create create folder [{0}], as chosen name is invalid", name);

                const Sierra::Path absolutePath = rootDirectoryPath / relativeDirectoryPath / name;
                if (applicationFrame.platformContext.GetFileManager().PathExists(absolutePath))
                    return SR_FORMAT("Cannot create folder [{0}], as this path already exists", name);

                return { };
            },
            .ActionCallback = [this, &applicationFrame](const std::string_view name) -> bool
            {
                const Sierra::Path absolutePath = rootDirectoryPath / relativeDirectoryPath / name;
                if (const Sierra::PathResult result = applicationFrame.platformContext.GetFileManager().CreateDirectory(absolutePath, Sierra::PathConflictPolicy::Overwrite); result != Sierra::PathResult::Success)
                {
                    APP_WARNING("Could not create folder [{0}] due to Sierra::PathOperationResult error code [{1}]", name, static_cast<uint32>(result));
                    return false;
                }

                visibleEntryIndices.emplace_back(visibleEntryIndices.size());
                AddEntry(applicationFrame, relativeDirectoryPath / name);

                return true;
            }
        };

        std::unique_ptr<Modal> modal = std::make_unique<TextDialog>(createInfo);
        editorFrame.modalManager.PushModal(std::move(modal));

        return true;
    }

    bool AssetBrowserPanel::OpenRenameDialog(const ApplicationFrame& applicationFrame, const EditorFrame& editorFrame)
    {
        if (IsSelectionEmpty())
            return false;

        const std::string placeholder = GetSelectedEntry()->relativePath.GetStem();
        const TextDialogCreateInfo createInfo
        {
            .title = "Rename",
            .text = "New name:",
            .placeholder = placeholder,
            .ValidationCallback = [this, &applicationFrame](const std::string_view name) -> std::string
            {
                 if (!Sierra::IsPathNameValid(name))
                     return SR_FORMAT("Cannot rename to [{0}], as chosen name is invalid", name);

                const AssetBrowserEntry& entry = *GetSelectedEntry();
                const Sierra::Path absolutePath = rootDirectoryPath / entry.relativePath.GetParentPath() / (std::string(name) + entry.relativePath.GetExtension());
                if (applicationFrame.platformContext.GetFileManager().PathExists(absolutePath))
                    return SR_FORMAT("Cannot rename to [{0}], as this path already exists", name);

                return { };
            },
            .ActionCallback = [this, &applicationFrame](const std::string_view name) -> bool
            {
                AssetBrowserEntry& entry = *GetSelectedEntry();
                const Sierra::Path& absolutePath = rootDirectoryPath / entry.relativePath;

                const std::string newName = std::string(name);
                if (const Sierra::PathResult result = applicationFrame.platformContext.GetFileManager().RenamePath(absolutePath, newName); result != Sierra::PathResult::Success)
                {
                    APP_WARNING("Could not rename [{0}] to [{1}] due to Sierra::PathOperationResult error code [{2}]", absolutePath.GetString(), name, static_cast<uint32>(result));
                    return false;
                }

                const size extensionSize = newName.size() - name.size();
                entry.name = newName.substr(0, newName.size() - extensionSize);
                entry.relativePath.ChangeFileName(newName);

                return true;
            }
        };

        std::unique_ptr<Modal> modal = std::make_unique<TextDialog>(createInfo);
        editorFrame.modalManager.PushModal(std::move(modal));

        return true;
    }

    bool AssetBrowserPanel::OpenDeleteDialog(const ApplicationFrame& applicationFrame, const EditorFrame& editorFrame)
    {
        if (IsSelectionEmpty())
            return false;

        const ConfirmationDialogCreateInfo createInfo
        {
            .title = "Delete Items",
            .text = "Permanently delete the selected items? This cannot be undone.",
            .confirmationText = "Delete",
            .ActionCallback = [this, &applicationFrame]() -> void { DeleteSelection(applicationFrame); }
        };


        std::unique_ptr<Modal> modal = std::make_unique<ConfirmationDialog>(createInfo);
        editorFrame.modalManager.PushModal(std::move(modal));

        return true;
    }

    /* --- GETTER METHODS --- */

    bool AssetBrowserPanel::IsSelected(const Sierra::Path& relativePath) const noexcept
    {
        const auto iterator = std::ranges::find_if(selectedEntryIndices, [this, &relativePath](const size index) -> bool { return entries[index].relativePath == relativePath; });
        return iterator != selectedEntryIndices.end();
    }

    /* --- PRIVATE METHODS --- */

    void AssetBrowserPanel::HandleShortcuts(const ApplicationFrame& applicationFrame, const EditorFrame& editorFrame)
    {
        if (applicationFrame.inputManager == nullptr)
            return;

        const Sierra::InputManager& inputManager = *applicationFrame.inputManager;
        if (inputManager.IsKeyCombinationPressed(NEW_FOLDER_SHORTCUT)) OpenNewFolderDialog(applicationFrame, editorFrame);
        if (inputManager.IsKeyCombinationPressed(REFRESH_SHORTCUT)) Refresh(applicationFrame);

        if (inputManager.IsKeyCombinationPressed(OPEN_SHORTCUT)) OpenSelection(applicationFrame);
        if (inputManager.IsKeyCombinationPressed(RENAME_SHORTCUT)) OpenRenameDialog(applicationFrame, editorFrame);
        if (inputManager.IsKeyCombinationPressed(DELETE_SHORTCUT)) OpenDeleteDialog(applicationFrame, editorFrame);
    }

    void AssetBrowserPanel::RefreshEntryVisibility()
    {
        visibleEntryIndices.clear();
        for (size i = 0; i < entries.size(); i++)
        {
            const AssetBrowserEntry& entry = entries[i];

            bool matchesFilter = entry.type == AssetBrowserEntryType::Folder;
            switch (static_cast<FilterType>(filterIndex))
            {
                case FilterType::None:
                {
                    matchesFilter = true;
                    break;
                }
                case FilterType::Folders:
                {
                    matchesFilter = entry.type == AssetBrowserEntryType::Folder;
                    break;
                }
                case FilterType::Textures:
                {
                    matchesFilter = entry.type == AssetBrowserEntryType::Texture;
                    break;
                }
                case FilterType::Models:
                {
                    matchesFilter = entry.type == AssetBrowserEntryType::Model;
                    break;
                }
                case FilterType::Materials:
                {
                    matchesFilter = entry.type == AssetBrowserEntryType::Material;
                    break;
                }
                case FilterType::Images:
                {
                    matchesFilter = entry.type == AssetBrowserEntryType::Image;
                    break;
                }
                case FilterType::Other:
                {
                    matchesFilter = entry.type == AssetBrowserEntryType::Unknown;
                    break;
                }
            }

            if (matchesFilter)
            {
                visibleEntryIndices.emplace_back(i);
            }
        }

        RefreshEntryOrder();
    }

    void AssetBrowserPanel::RefreshEntryOrder()
    {
        const auto Compare = [this](const AssetBrowserEntry& left, const AssetBrowserEntry& right) -> int32
        {
            int32 comparison = 0;
            switch (static_cast<SortType>(sortIndex))
            {
                case SortType::Type:
                {
                    comparison = static_cast<int32>(left.type) - static_cast<int32>(right.type);
                    break;
                }
                case SortType::Modified:
                {
                    if (left.timeLastModified < right.timeLastModified) comparison = -1;
                    else if (right.timeLastModified < left.timeLastModified) comparison = 1;
                    break;
                }
                case SortType::Size:
                {
                    if (left.memorySize < right.memorySize) comparison = -1;
                    else if (left.memorySize > right.memorySize) comparison = 1;
                    break;
                }
                default:
                {
                    break;
                }
            }

            if (comparison == 0) comparison = ToLower(left.name).compare(ToLower(right.name));
            return comparison;
        };

        std::ranges::sort(visibleEntryIndices, [this, &Compare](const size left, const size right) -> bool
        {
            const AssetBrowserEntry& leftEntry = entries[left];
            const AssetBrowserEntry& rightEntry = entries[right];

            // Folders are always listed first, regardless of the direction of sorting
            const bool leftFolder = leftEntry.type == AssetBrowserEntryType::Folder;
            const bool rightFolder = rightEntry.type == AssetBrowserEntryType::Folder;
            if (leftFolder != rightFolder) return leftFolder;

            const int32 comparison = Compare(leftEntry, rightEntry);
            return sortDescending ? comparison > 0 : comparison < 0;
        });
    }

    void AssetBrowserPanel::UpdateRelativeDirectoryPath(const ApplicationFrame& applicationFrame, const Sierra::Path& givenRelativeDirectoryPath)
    {
        relativeDirectoryPath = givenRelativeDirectoryPath;
        Refresh(applicationFrame);

        searchText.clear();
        revealCurrentDirectory = true;
    }

    void AssetBrowserPanel::DrawNavigation(const ApplicationFrame& applicationFrame, const EditorFrame& editorFrame)
    {
        const ImGuiStyle& style = ImGui::GetStyle();

        ImGui::BeginDisabled(backHistory.empty());
        if (ImGui::ArrowButton("##Back", ImGuiDir_Left)) NavigateBack(applicationFrame);
        ImGui::EndDisabled();

        ImGui::SameLine();
        ImGui::BeginDisabled(forwardHistory.empty());
        if (ImGui::ArrowButton("##Forward", ImGuiDir_Right)) NavigateForward(applicationFrame);
        ImGui::EndDisabled();

        ImGui::SameLine();
        ImGui::BeginDisabled(relativeDirectoryPath.IsEmpty());
        if (ImGui::ArrowButton("##Up", ImGuiDir_Up)) NavigateUp(applicationFrame);
        ImGui::EndDisabled();

        // Breadcrumbs, where every folder of the current path can be opened
        ImGui::SameLine(0.0f, style.ItemSpacing.x * 2.0f);

        int32 i = 0;
        const std::string rootDirectoryName = rootDirectoryPath.GetStem();
        if (DrawBreadcrumb(i, rootDirectoryName, true)) NavigateTo(applicationFrame, { });
        i++;

        Sierra::Path currentRelativeDirectoryPath = { };
        for (const Sierra::Path segment : relativeDirectoryPath)
        {
            currentRelativeDirectoryPath /= segment;

            const std::string  directoryName = segment.GetStem();
            if (DrawBreadcrumb(i, directoryName)) NavigateTo(applicationFrame, currentRelativeDirectoryPath);

            i++;
        }

        // Actions, aligned to the right
        const auto ButtonWidth = [&style](const char* label) -> float32 { return ImGui::CalcTextSize(label).x + style.FramePadding.x * 2.0f; };
        const float32 actionsWidth = ButtonWidth("New Folder") + ButtonWidth("Refresh") + style.ItemSpacing.x * 2.0f;

        ImGui::SameLine(std::max(ImGui::GetCursorPosX() + style.ItemSpacing.x, ImGui::GetContentRegionMax().x - actionsWidth));
        if (ImGui::Button("New Folder")) OpenNewFolderDialog(applicationFrame, editorFrame);

        ImGui::SameLine();
        if (ImGui::Button("Refresh"))
        {
            Refresh(applicationFrame);
        }
    }

    void AssetBrowserPanel::DrawToolBar(const ApplicationFrame& applicationFrame)
    {
        const ImGuiStyle& style = ImGui::GetStyle();

        uint32 modifiedFilterIndex = filterIndex;
        if (ImGuiWidgets::Dropdown("##FilterDropdown", modifiedFilterIndex, { .options = FILTER_OPTIONS, .width = 112.0f }))
        {
            filterIndex = modifiedFilterIndex;
            RefreshEntryVisibility();
        }

        bool sortChanged = false;

        ImGui::SameLine();
        uint32 modifiedSortIndex = sortIndex;
        if (ImGuiWidgets::Dropdown("##SortDropdown", modifiedSortIndex, { .options = SORT_OPTIONS, .width = 96.0f }))
        {
            sortChanged = modifiedSortIndex != sortIndex;
            sortIndex = modifiedSortIndex;
        }

        ImGui::SameLine();
        if (ImGui::ArrowButton("##SortDirection", sortDescending ? ImGuiDir_Down : ImGuiDir_Up))
        {
            sortDescending = !sortDescending;
            sortChanged = true;
        }

        if (sortChanged)
            RefreshEntryOrder();

        ImGui::SameLine();
        if (ImGuiWidgets::TextInput("##SearchInput", searchText, { .width = 200.0f }))
        {
            Refresh(applicationFrame);
        }

        // View mode and tile size
        const auto ToggleButton = [](const char* label, const bool active) -> bool
        {
            if (active) ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive));
            const bool pressed = ImGui::Button(label);
            if (active) ImGui::PopStyleColor();
            return pressed;
        };

        ImGui::SameLine(0.0f, style.ItemSpacing.x * 2.0f);
        if (ToggleButton("Grid", !listView)) listView = false;
        ImGui::SameLine();
        if (ToggleButton("List", listView)) listView = true;

        if (!listView)
        {
            ImGui::SameLine();
            ImGui::SetNextItemWidth(120.0f);
            ImGui::SliderFloat("##TileSize", &tileSize, MIN_TILE_SIZE, MAX_TILE_SIZE, "%.0f px");
        }

        // Details and options, aligned to the right
        const float32 optionsWidth = ImGui::CalcTextSize("Options").x + style.FramePadding.x * 2.0f;
        const float32 detailsWidth = ImGui::CalcTextSize("Details").x + style.FramePadding.x * 2.0f + style.ItemInnerSpacing.x + ImGui::GetFrameHeight();
        ImGui::SameLine(std::max(ImGui::GetCursorPosX(), ImGui::GetContentRegionMax().x - optionsWidth - detailsWidth));

        ImGui::Checkbox("Details", &showDetails);

        ImGui::SameLine();
        if (ImGui::Button("Options")) ImGui::OpenPopup("##OptionsMenu");
        if (ImGui::BeginPopup("##OptionsMenu"))
        {
            ImGui::MenuItem("Show extensions", nullptr, &showExtensions);
            if (ImGui::MenuItem("Show hidden entries", nullptr, &showHiddenEntries))  RefreshEntryVisibility();
            if (ImGui::MenuItem("Show companion .blob files", nullptr, &showCompanionFiles)) RefreshEntryVisibility();
            ImGui::EndPopup();
        }
    }

    void AssetBrowserPanel::DrawTree(const ApplicationFrame& applicationFrame, const float32 height)
    {
        if (ImGui::BeginChild("##Tree", ImVec2(TREE_WIDTH, height), ImGuiChildFlags_Borders))
        {
            DrawTreeNode(applicationFrame, { });
        }
        ImGui::EndChild();

        revealCurrentDirectory = false;
    }

    void AssetBrowserPanel::DrawTreeNode(const ApplicationFrame& applicationFrame, const Sierra::Path& givenRelativeDirectoryPath)
    {
        const std::string directoryName = givenRelativeDirectoryPath.IsEmpty() ? rootDirectoryPath.GetStem() : givenRelativeDirectoryPath.GetStem();
        const Sierra::Path absoluteDirectoryPath = rootDirectoryPath / givenRelativeDirectoryPath;

        const std::string idString = absoluteDirectoryPath.GetString();
        ImGui::PushID(idString.c_str(), idString.c_str() + idString.size());

        const bool current = givenRelativeDirectoryPath == relativeDirectoryPath;
        const bool ancestor = givenRelativeDirectoryPath.IsAncestor(relativeDirectoryPath);

        const std::vector<Sierra::Path> children = ListChildDirectoryPaths(applicationFrame.platformContext.GetFileManager(), absoluteDirectoryPath);
        const ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DrawLinesToNodes | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth | (givenRelativeDirectoryPath.IsEmpty() * ImGuiTreeNodeFlags_DefaultOpen) | (current * ImGuiTreeNodeFlags_Selected) | (children.empty() * ImGuiTreeNodeFlags_Leaf);

        // Folders leading to the current one are opened when navigating to it
        if (revealCurrentDirectory && ancestor && !current) ImGui::SetNextItemOpen(true);

        const bool open = ImGui::TreeNodeEx("##Node", flags, "%s", directoryName.c_str());
        if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && !ImGui::IsItemToggledOpen())
        {
            NavigateTo(applicationFrame, givenRelativeDirectoryPath);
        }

        if (open)
        {
            for (const Sierra::Path& child : children)
            {
                DrawTreeNode(applicationFrame, child.GetRelativePath(rootDirectoryPath));
            }

            ImGui::TreePop();
        }

        ImGui::PopID();
    }

    void AssetBrowserPanel::AddEntry(const ApplicationFrame& applicationFrame, const Sierra::Path& relativeEntryPath)
    {
        const Sierra::Path absoluteEntryPath = rootDirectoryPath / relativeEntryPath;

        AssetBrowserEntry entry = { };
        entry.name = relativeEntryPath.GetFileName();
        entry.relativePath = relativeEntryPath;
        entry.type = std::filesystem::is_directory(absoluteEntryPath) ? AssetBrowserEntryType::Folder : ExtensionToEntryType(relativeEntryPath.GetExtension());

        Sierra::PathMetadata metadata = { };
        if (applicationFrame.platformContext.GetFileManager().GetPathMetadata(absoluteEntryPath, metadata) != Sierra::PathResult::Success)
        {
            APP_WARNING("Could not retrieve metadata of path [{0}]", absoluteEntryPath.GetString());
        }
        else
        {
            entry.memorySize = metadata.memorySize;
            entry.timeCreated = metadata.timeCreated;
            entry.timeLastModified = metadata.timeLastModified;
        }

        AddEntry(std::move(entry));
    }

    void AssetBrowserPanel::AddEntry(AssetBrowserEntry&& entry)
    {
        entries.emplace_back(std::move(entry));
    }

    void AssetBrowserPanel::DrawEntries(const ApplicationFrame& applicationFrame, const Vector2 size)
    {
        if (ImGui::BeginChild("##AssetEntries", size, ImGuiChildFlags_Borders))
        {
            if (entries.empty())
            {
                DrawEmptyEntries();
            }
            else if (listView)
            {
                DrawListEntries(applicationFrame);
            }
            else
            {
                DrawGridEntries(applicationFrame);
            }

            // Clicks on empty space clear the selection, and right clicks open the menu of the folder
            if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered())
            {
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) ClearSelection();
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) openBackgroundContextMenu = true;
            }
        }
        ImGui::EndChild();
    }

    void AssetBrowserPanel::DrawEmptyEntries()
    {
        const std::string message = searchText.empty() ? std::string("This folder is empty") : SR_FORMAT("No assets match [{0}]", searchText);
        const std::string_view hint = searchText.empty() ? "Add items to the folder or right-click to create them now" : "Clear the search to browse this folder";

        const ImVec2 available = ImGui::GetContentRegionAvail();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + available.y * 0.35f);

        ImGui::SetCursorPosX(std::max(0.0f, (available.x - ImGui::CalcTextSize(message.c_str()).x) * 0.5f));
        ImGui::TextUnformatted(message.c_str());

        ImGui::SetCursorPosX(std::max(0.0f, (available.x - ImGui::CalcTextSize(hint.data()).x) * 0.5f));
        ImGui::TextDisabled("%s", hint.data());
    }

    void AssetBrowserPanel::DrawGridEntries(const ApplicationFrame& applicationFrame)
    {
        const float32 fontSize = ImGui::GetFontSize();
        const float32 spacing = ImGui::GetStyle().ItemSpacing.x;
        const ImVec2 tileSizeVector = { tileSize + 2.0f * TILE_PADDING, TILE_PADDING + tileSize + 6.0f + fontSize * 2.0f + 3.0f + TILE_PADDING };

        const float32 availableWidth = ImGui::GetContentRegionAvail().x;
        const size columns = glm::max<size>(1, (availableWidth + spacing) / (tileSizeVector.x + spacing));

        for (size i = 0; i < visibleEntryIndices.size(); i++)
        {
            const AssetBrowserEntry& entry = entries[visibleEntryIndices[i]];
            if ((entry.relativePath.IsHidden() && !showHiddenEntries) || (entry.type == AssetBrowserEntryType::Blob && !showCompanionFiles))
            {
                continue;
            }

            if (i % columns != 0) ImGui::SameLine();

            ImGui::PushID(static_cast<int>(i));
            DrawGridEntry(applicationFrame, entry, tileSizeVector);
            ImGui::PopID();
        }
    }

    void AssetBrowserPanel::DrawGridEntry(const ApplicationFrame& applicationFrame, const AssetBrowserEntry& entry, const Vector2 size)
    {
        const ImVec2 origin = ImGui::GetCursorScreenPos();
        ImGui::InvisibleButton("##Tile", size);

        const bool visible = ImGui::IsItemVisible();
        const bool hovered = ImGui::IsItemHovered();
        HandleEntryInteraction(applicationFrame, entry);
        if (!visible) return;

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        const ImVec2 max = {origin.x + size.x, origin.y + size.y };
        const bool selected = IsSelected(entry.relativePath);

        if (selected)
        {
            drawList->AddRectFilled(origin, max, ImGui::GetColorU32(ImGuiCol_Header), TILE_ROUNDING);
            drawList->AddRect(origin, max, ImGui::GetColorU32(ImGuiCol_HeaderActive), TILE_ROUNDING, 0, 1.5f);
        }
        else if (hovered)
        {
            drawList->AddRectFilled(origin, max, ImGui::GetColorU32(ImGuiCol_HeaderHovered), TILE_ROUNDING);
        }

        const ImVec2 thumbnailMin = { origin.x + TILE_PADDING, origin.y + TILE_PADDING };
        const ImVec2 thumbnailMax = { thumbnailMin.x + tileSize, thumbnailMin.y + tileSize };
        DrawItemGlyph(drawList, thumbnailMin, thumbnailMax, entry.type);

        ImFont* font = ImGui::GetFont();
        const float32 fontSize = ImGui::GetFontSize();
        const float32 textWidth = size.x - 2.0f * TILE_PADDING;

        const ImVec2 namePosition = { origin.x + TILE_PADDING, thumbnailMax.y + 6.0f };

        const std::string name = ImGuiWidgets::TruncateText(fontSize, showExtensions ? entry.name : Sierra::Path(entry.name).RemoveExtension().GetString(), textWidth);
        drawList->AddText(font, fontSize, namePosition, ImGui::GetColorU32(ImGuiCol_Text), name.c_str());

        const ImVec2 subtitlePosition = { namePosition.x, namePosition.y + fontSize + 3.0f };
        const std::string subtitle = ImGuiWidgets::TruncateText(fontSize * 0.85f, BuildSubtitle(entry), textWidth);
        drawList->AddText(font, fontSize * 0.85f, subtitlePosition, ImGui::GetColorU32(ImGuiCol_TextDisabled), subtitle.c_str());
    }

    void AssetBrowserPanel::DrawListEntries(const ApplicationFrame& applicationFrame)
    {
        constexpr ImGuiTableFlags TABLE_FLAGS = ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingStretchProp;
        if (!ImGui::BeginTable("##AssetList", 4, TABLE_FLAGS)) return;

        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch, 3.0f);
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 90.0f);
        ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("Modified", ImGuiTableColumnFlags_WidthFixed, 130.0f);
        ImGui::TableHeadersRow();

        for (size i = 0; i < visibleEntryIndices.size(); i++)
        {
            const AssetBrowserEntry& entry = entries[visibleEntryIndices[i]];
            if ((Sierra::Path(entry.name).IsHidden() && !showHiddenEntries) || (entry.type == AssetBrowserEntryType::Blob && !showCompanionFiles))
            {
                continue;
            }

            ImGui::PushID(static_cast<int>(i));
            DrawListEntry(applicationFrame, entry);
            ImGui::PopID();
        }

        ImGui::EndTable();
    }

    void AssetBrowserPanel::DrawListEntry(const ApplicationFrame& applicationFrame, const AssetBrowserEntry& entry)
    {
        ImFont* font = ImGui::GetFont();
        const float32 fontSize = ImGui::GetFontSize();

        ImGui::TableNextRow(0, LIST_ROW_HEIGHT);
        ImGui::TableSetColumnIndex(0);

        ImGui::Selectable("##Row", IsSelected(entry.relativePath), ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap, ImVec2(0.0f, LIST_ROW_HEIGHT));

        const bool visible = ImGui::IsItemVisible();
        const ImGuiStyle& style = ImGui::GetStyle();
        const ImVec2 rowMin = ImGui::GetItemRectMin() + ImVec2(0, style.ItemSpacing.y / 2.0f);
        const ImVec2 rowMax = ImGui::GetItemRectMax();
        HandleEntryInteraction(applicationFrame, entry);

        if (visible)
        {
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            const float32 iconTop = rowMin.y + (LIST_ROW_HEIGHT - LIST_ICON_SIZE) * 0.5f;
            DrawItemGlyph(drawList, { rowMin.x + 4.0f, iconTop }, { rowMin.x + 4.0f + LIST_ICON_SIZE, iconTop + LIST_ICON_SIZE }, entry.type);

            const float32 textLeft = rowMin.x + 4.0f + LIST_ICON_SIZE + 8.0f;
            const float32 textWidth = rowMax.x - textLeft - 8.0f;

            const std::string name = ImGuiWidgets::TruncateText(fontSize, showExtensions ? entry.name : Sierra::Path(entry.name).RemoveExtension().GetString(), textWidth, font);
            drawList->AddText(font, fontSize, { textLeft, rowMin.y + (LIST_ROW_HEIGHT - fontSize) * 0.5f }, ImGui::GetColorU32(ImGuiCol_Text), name.c_str());
        }

        ImGui::TableSetColumnIndex(1);
        const std::string_view typeLabel = AssetBrowserEntryTypeToString(entry.type);
        ImGui::TextUnformatted(typeLabel.data(), typeLabel.data() + typeLabel.size());

        ImGui::TableSetColumnIndex(2);
        ImGui::TextUnformatted(entry.memorySize > 0 ? FormatSize(entry.memorySize).c_str() : "-");

        ImGui::TableSetColumnIndex(3);
        ImGui::TextUnformatted(Sierra::TimePointToString(entry.timeLastModified).c_str());
    }

    void AssetBrowserPanel::HandleEntryInteraction(const ApplicationFrame& applicationFrame, const AssetBrowserEntry& entry)
    {
        if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
        {
            Select(applicationFrame, entry.relativePath);
        }

        if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
        {
            if (!IsSelected(entry.relativePath)) Select(applicationFrame, entry.relativePath);
            openSelectionContextMenu = true;
        }

        if (entry.type == AssetBrowserEntryType::Folder && ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
            // NOTE: Navigating replaces all entries, including this one, which is still being drawn, so it is postponed until drawing of entries is done
            pendingNavigationPath = entry.relativePath;
        }

        if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip | ImGuiHoveredFlags_DelayNormal) && !ImGui::IsMouseDragging(ImGuiMouseButton_Left))
        {
            ImGui::BeginTooltip();
            ImGui::TextUnformatted(entry.name.c_str());
            ImGui::TextDisabled("%s", BuildSubtitle(entry).c_str());
            ImGui::EndTooltip();
        }
    }

    void AssetBrowserPanel::DrawDetails(const float32 height)
    {
        if (ImGui::BeginChild("##AssetDetails", ImVec2(DETAILS_WIDTH, height), ImGuiChildFlags_Borders))
        {
            if (selectedEntryIndices.size() == 1)
            {
                const AssetBrowserEntry* entry = GetSelectedEntry();
                if (entry->type != AssetBrowserEntryType::Folder)
                {
                    DrawFileEntryDetails(*entry);
                }
                else
                {
                    DrawFolderDetails();
                }
            }
            else if (selectedEntryIndices.size() > 1)
            {
                size totalSize = 0;
                for (const size index : selectedEntryIndices)
                {
                    const AssetBrowserEntry& entry = entries[index];
                    totalSize += entry.memorySize;
                }

                ImGui::Spacing();
                ImGui::Text("%zu items selected", selectedEntryIndices.size());
                ImGui::Separator();
                DrawDetailRow("Total size", FormatSize(totalSize));
            }
            else
            {
                DrawFolderDetails();
            }
        }
        ImGui::EndChild();
    }

    void AssetBrowserPanel::DrawFolderDetails()
    {
        size folderCount = 0;
        size fileCount = 0;
        size totalSize = 0;
        for (const size index : visibleEntryIndices)
        {
            const AssetBrowserEntry& entry = entries[index];
            if (entry.type == AssetBrowserEntryType::Folder)
            {
                folderCount++;
            }
            else
            {
                fileCount++;
                totalSize += entry.memorySize;
            }
        }

        const std::string folderName = relativeDirectoryPath.IsEmpty() ? rootDirectoryPath.GetFileName() : relativeDirectoryPath.GetFileName();
        ImGui::Spacing();
        ImGui::TextWrapped("%s", folderName.c_str());
        ImGui::TextDisabled("Folder");
        ImGui::Separator();

        const Sierra::Path absolutePath = rootDirectoryPath / relativeDirectoryPath;
        DrawDetailRow("Path", absolutePath, true);
        DrawDetailRow("Folders", SR_FORMAT("{0}", folderCount));
        DrawDetailRow("Files", SR_FORMAT("{0}", fileCount));
        DrawDetailRow("Total size", FormatSize(totalSize));
    }

    void AssetBrowserPanel::DrawFileEntryDetails(const AssetBrowserEntry& entry)
    {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        const ImVec2 previewMin = ImGui::GetCursorScreenPos();
        const ImVec2 previewMax = { previewMin.x + DETAILS_PREVIEW_SIZE, previewMin.y + DETAILS_PREVIEW_SIZE };
//        DrawEntryThumbnail(drawList, entry, previewMin, previewMax);
        DrawItemGlyph(drawList, previewMin, previewMax, entry.type);
        ImGui::Dummy({ DETAILS_PREVIEW_SIZE, DETAILS_PREVIEW_SIZE });

        ImGui::Spacing();
        ImGui::TextWrapped("%s", entry.name.c_str());
        ImGui::TextDisabled("%s", AssetBrowserEntryTypeToString(entry.type).data());
        ImGui::Separator();

        if (entry.type != AssetBrowserEntryType::Folder) DrawDetailRow("Size", FormatSize(entry.memorySize));
        DrawDetailRow("Modified", Sierra::TimePointToString(entry.timeLastModified));
        DrawDetailRow("Location", entry.relativePath.GetParentPath());

        switch (entry.type)
        {
            case AssetBrowserEntryType::Unknown:
            case AssetBrowserEntryType::Folder:
            case AssetBrowserEntryType::Blob:
            {
                return;
            }
            default:
            {
                break;
            }
        }
    }

    void AssetBrowserPanel::UpdateStatus(const std::string& message, const Sierra::AlertSeverity severity)
    {
        statusMessage = std::move(message);
        statusSeverity = severity;
        statusMessageTime = ImGui::GetTime();
    }

    void AssetBrowserPanel::DrawStatusBar()
    {
        ImGui::Separator();

        ImGui::TextDisabled("%zu items", visibleEntryIndices.size());
        if (!selectedEntryIndices.empty())
        {
            ImGui::SameLine();
            ImGui::TextDisabled("|  %zu selected", selectedEntryIndices.size());
        }

        if (!statusMessage.empty() && ImGui::GetTime() - statusMessageTime < STATUS_MESSAGE_DURATION)
        {
            ImGui::SameLine();

            ImColor color = { };
            switch (statusSeverity)
            {
                case Sierra::AlertSeverity::Info:    {  color = INFO_COLOR; break; }
                case Sierra::AlertSeverity::Warning: {  color = WARNING_COLOR; break; }
                case Sierra::AlertSeverity::Error:   {  color = ERROR_COLOR; break; }
                default:                     break;
            }

            ImGui::TextColored(color, "|  %s", statusMessage.c_str());
        }
    }

    void AssetBrowserPanel::DrawPopups(const ApplicationFrame& applicationFrame, const EditorFrame& editorFrame)
    {
        if (openBackgroundContextMenu)
        {
            ImGui::OpenPopup("##BackgroundContextMenu");
            openBackgroundContextMenu = false;
        }

        if (openSelectionContextMenu)
        {
            ImGui::OpenPopup("##SelectionContextMenu");
            openSelectionContextMenu = false;
        }

        if (ImGui::BeginPopup("##BackgroundContextMenu"))
        {
            DrawBackgroundContextMenu(applicationFrame, editorFrame);
            ImGui::EndPopup();
        }

        if (ImGui::BeginPopup("##SelectionContextMenu"))
        {
            DrawSelectionContextMenu(applicationFrame, editorFrame);
            ImGui::EndPopup();
        }
    }

    void AssetBrowserPanel::DrawBackgroundContextMenu(const ApplicationFrame& applicationFrame, const EditorFrame& editorFrame)
    {
        if (ImGui::MenuItem("New Folder", Sierra::KeyCombinationToString(NEW_FOLDER_SHORTCUT).data()))
            OpenNewFolderDialog(applicationFrame, editorFrame);

        ImGui::Separator();

        if (ImGui::MenuItem("Refresh", Sierra::KeyCombinationToString(REFRESH_SHORTCUT).data()))
            Refresh(applicationFrame);
    }

    void AssetBrowserPanel::DrawSelectionContextMenu(const ApplicationFrame& applicationFrame, const EditorFrame& editorFrame)
    {
        const bool singleEntry = selectedEntryIndices.size() == 1;

        const AssetBrowserEntry* entry = GetSelectedEntry();
        const bool entryIsFolder = singleEntry && entry->type == AssetBrowserEntryType::Folder;

        if (singleEntry && ImGui::MenuItem("Open", Sierra::KeyCombinationToString(OPEN_SHORTCUT).data(), false, entryIsFolder)) OpenSelection(applicationFrame);
        if (ImGui::MenuItem("Rename", Sierra::KeyCombinationToString(RENAME_SHORTCUT).data(), false, singleEntry)) OpenRenameDialog(applicationFrame, editorFrame);

        ImGui::Separator();
        if (!searchText.empty())
        {
            if (ImGui::MenuItem("Go to containing folder")) NavigateTo(applicationFrame, entry->relativePath.GetParentPath());
            ImGui::Separator();
        }

        if (ImGui::MenuItem("Delete", Sierra::KeyToString(Sierra::Key::Delete).data(), false, !selectedEntryIndices.empty())) OpenDeleteDialog(applicationFrame, editorFrame);
    }

}