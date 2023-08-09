//
// Created by Nikolay Kanchevski on 30.07.23.
//

#include "AssetBrowserPanel.h"

#include "../GUI.h"
#include "../Editor.h"
#include "../../Engine/Handlers/Project.h"
#include "../../Engine/Handlers/Assets/AssetManager.h"

namespace Sierra::Editor
{
    using namespace Engine;

    constexpr float ITEM_SIZE = 72.5f;
    FilePath currentDirectory = "";

    /* --- POLLING METHODS --- */

    void DrawFilePath(const FilePath &filePath)
    {
        // Draw file icon
        Vector2 UV0, UV1;
        auto iconAtlas = AssetManager::GetEngineIconCollection().GetEngineIcon(is_directory(filePath) ? EngineIconType::DIRECTORY : EngineIconType_FromFileExtension(filePath.extension().c_str()), UV0, UV1);
        ImGui::Image(iconAtlas, { ITEM_SIZE, ITEM_SIZE }, *reinterpret_cast<ImVec2*>(&UV0), *reinterpret_cast<ImVec2*>(&UV1));

        if (is_directory(filePath) && GUI::IsItemDoubleClicked())
        {
            currentDirectory /= filePath.filename();
        }
        else if (!is_directory(filePath) && GUI::IsItemClicked())
        {
            Editor::SetSelectedAsset(AssetID(filePath));
        }

        GUI::PushFontSize(GUI::GetFontSize() * 0.85f);
        auto textSize = ImGui::CalcTextSize(filePath.filename().c_str());
        if (textSize.x > ImGui::GetItemRectSize().x)
        {
            ImGui::RenderTextEllipsis(ImGui::GetForegroundDrawList(), { ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y }, { ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y + 25.0f }, ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().x - 0.5f, filePath.filename().c_str(), nullptr, nullptr);
        }
        else
        {
            ImGui::RenderText({ ImGui::GetItemRectMin().x + (ImGui::GetItemRectSize().x - textSize.x) * 0.5f, ImGui::GetItemRectMax().y }, filePath.filename().c_str());
        }
        GUI::PopFontSize();

        GUI::VerticalIndent(25.0f);
        ImGui::NextColumn();
    }

    void AssetBrowserPanel::DrawUI()
    {
        if (GUI::BeginWindow("Asset Browser"))
        {
            // Get all directories for current path and manually add the 'Assets/' bit
            auto directories = File::GetDirectoriesFromPath(currentDirectory);
            directories.insert(directories.begin(), "Assets");

            ImGui::BeginGroup();

            // Draw every subpath
            GUI::PushStyleVariable(StyleVariable::FRAME_BORDER_SIZE, 0.0f);
            GUI::PushStyleVariable(StyleVariable::ITEM_SPACING, { 0.0f, GImGui->Style.ItemSpacing.y });
            for (uint32 i = 0; i < directories.size(); i++)
            {
                if (GUI::RoundedButton(directories[i].c_str(), { ImGui::CalcTextSize(directories[i].c_str()).x + 2.0f * GImGui->Style.ItemInnerSpacing.x, ImGui::GetItemRectSize().y }, i == 0 ? CornerRoundingFlags::LEFT : CornerRoundingFlags::NONE))
                {
                    for (uint32 j = directories.size() - 1 - i; j--;)
                    {
                        currentDirectory = currentDirectory.parent_path();
                    }
                }
                GUI::ContinueOnSameLine();

                GUI::PushStyleColor(StyleColor::HOVERED_BUTTON, *reinterpret_cast<Vector4*>(&GImGui->Style.Colors[static_cast<uint32>(StyleColor::BUTTON)]));
                GUI::RoundedButton("/", { ImGui::GetFrameHeight(), ImGui::GetItemRectSize().y }, i == directories.size() - 1 ? CornerRoundingFlags::RIGHT : CornerRoundingFlags::NONE);
                GUI::PopStyleColor();

                if (i != directories.size() - 1) GUI::ContinueOnSameLine();
            }
            GUI::PopStyleVariable(2);

            ImGui::EndGroup();

            // Calculate sizes
            float cellSize = ITEM_SIZE + 2 * 6.0f;
            int32 columnCount = static_cast<int32>(GUI::GetRemainingHorizontalSpace() / cellSize);
            if (columnCount < 1) { columnCount = 1; }

            // Begin table
            ImGui::Columns(columnCount, nullptr, false);

            // Draw every file/directory
            for (const auto &entry : std::filesystem::directory_iterator(Project::GetAssetDirectory() / currentDirectory))
            {
                if (entry.path().filename().c_str()[0] == '.') continue;
                DrawFilePath(entry.path());
            }
        }
        GUI::EndWindow();
    }
}