//
// Created by Nikolay Kanchevski on 15.11.26.
//

#include "LogPanel.h"

#include <list>
#include <mutex>
#include <unordered_map>

#include <spdlog/sinks/base_sink.h>

#include "../Widgets/ImGuiWidgets.h"

namespace SierraEngine
{

    namespace
    {
        constexpr std::string_view PANEL_TITLE = "Log";
        constexpr size MAX_DISTINCT_ENTRY_COUNT = 1000;

        constexpr float32 SEARCH_WIDTH = 200.0f;
        constexpr float32 TIME_COLUMN_WIDTH = 72.0f;
        constexpr float32 SEVERITY_COLUMN_WIDTH = 80.0f;
        constexpr float32 ROW_HIGHLIGHT_ALPHA = 0.12f;

        constexpr float32 DETAILS_HEIGHT_RATIO = 0.35f;
        constexpr float32 DETAILS_MIN_HEIGHT = 96.0f;
        constexpr float32 DETAILS_MAX_HEIGHT = 260.0f;

        constexpr ImColor WARNING_COLOR = { 0.95f, 0.75f, 0.30f, 1.0f };
        constexpr ImColor ERROR_COLOR   = { 0.95f, 0.35f, 0.35f, 1.0f };

        LogPanelSeverity SpdlogLevelToSeverity(const spdlog::level::level_enum level) noexcept
        {
            switch (level)
            {
                case spdlog::level::trace:
                case spdlog::level::debug:    return LogPanelSeverity::Debug;
                case spdlog::level::info:     return LogPanelSeverity::Info;
                case spdlog::level::warn:     return LogPanelSeverity::Warning;
                case spdlog::level::err:
                case spdlog::level::critical: return LogPanelSeverity::Error;
                case spdlog::level::off:
                case spdlog::level::n_levels: break;
            }

            return LogPanelSeverity::Info;
        }

        std::string_view SeverityToString(const LogPanelSeverity severity) noexcept
        {
            switch (severity)
            {
                case LogPanelSeverity::Debug:   return "Debug";
                case LogPanelSeverity::Info:    return "Info";
                case LogPanelSeverity::Warning: return "Warning";
                case LogPanelSeverity::Error:   return "Error";
            }

            return "Unknown";
        }

        ImVec4 SeverityToColor(const LogPanelSeverity severity)
        {
            switch (severity)
            {
                case LogPanelSeverity::Debug:   return ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled);
                case LogPanelSeverity::Info:    return ImGui::GetStyleColorVec4(ImGuiCol_Text);
                case LogPanelSeverity::Warning: return WARNING_COLOR;
                case LogPanelSeverity::Error:   return ERROR_COLOR;
            }

            return ImGui::GetStyleColorVec4(ImGuiCol_Text);
        }

        std::string FormatTime(const Sierra::TimePoint timePoint)
        {
            return SR_FORMAT("{0:02}:{1:02}:{2:02}", static_cast<uint32>(timePoint.GetHour()), static_cast<uint32>(timePoint.GetMinute()), static_cast<uint32>(timePoint.GetSecond()));
        }

        std::string GetSummary(const std::string& message)
        {
            const size lineEnd = message.find('\n');
            if (lineEnd == std::string::npos) return message;

            return message.substr(0, lineEnd) + " ...";
        }

        std::string MakeMessageKey(const std::string_view message, const LogPanelSeverity severity)
        {
            std::string key = { };
            key.reserve(message.size() + 1);

            key += static_cast<char>(severity);
            key += message;
            return key;
        }

        void DrawCountPill(const uint64 count)
        {
            const ImGuiStyle& style = ImGui::GetStyle();

            // Larger counts are capped so the pill stays small
            const std::string label = count > 999 ? "999+" : SR_FORMAT("{0}", count);
            const ImVec2 textSize = ImGui::CalcTextSize(label.c_str());
            // The pill is exactly one text line high, so it never makes its row taller than the text
            const ImVec2 size = { textSize.x + style.FramePadding.x * 2.0f, ImGui::GetTextLineHeight() };

            ImGui::SameLine(ImGui::GetContentRegionAvail().x - (size.x + 2 * style.ItemSpacing.x));

            // Whole pixels, so the text is not nudged left by sub-pixel rounding
            const ImVec2 cursor = ImGui::GetCursorScreenPos();
            const ImVec2 origin = { std::round(cursor.x), std::round(cursor.y) };
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            drawList->AddRectFilled(origin, { origin.x + size.x, origin.y + size.y }, ImGui::GetColorU32(ImGuiCol_HeaderActive), size.y * 0.5f);
            drawList->AddText({ origin.x + (size.x - textSize.x) * 0.5f, origin.y + (size.y - textSize.y) * 0.5f }, ImGui::GetColorU32(ImGuiCol_Text), label.c_str());
            ImGui::Dummy(size);
        }

        // Draws a cross like the close button of an ImGui window at the cursor, and returns true when it is clicked
        bool DrawCrossButton(const char* ID)
        {
            const float32 size = ImGui::GetFrameHeight();
            const ImVec2 origin = ImGui::GetCursorScreenPos();

            const bool clicked = ImGui::InvisibleButton(ID, { size, size });
            const bool hovered = ImGui::IsItemHovered();

            const ImVec2 center = { origin.x + size * 0.5f, origin.y + size * 0.5f };
            const float32 radius = size * 0.25f;

            ImDrawList* drawList = ImGui::GetWindowDrawList();
            if (hovered) drawList->AddCircleFilled(center, size * 0.5f, ImGui::GetColorU32(ImGuiCol_ButtonHovered));

            const ImU32 color = ImGui::GetColorU32(hovered ? ImGuiCol_Text : ImGuiCol_TextDisabled);
            drawList->AddLine({ center.x - radius, center.y - radius }, { center.x + radius, center.y + radius }, color, 1.5f);
            drawList->AddLine({ center.x + radius, center.y - radius }, { center.x - radius, center.y + radius }, color, 1.5f);

            return clicked;
        }

        bool ContainsIgnoreCase(const std::string_view text, const std::string_view query)
        {
            const auto IsEqual = [](const char left, const char right) -> bool
            {
                return std::tolower(static_cast<unsigned char>(left)) == std::tolower(static_cast<unsigned char>(right));
            };

            return std::search(text.begin(), text.end(), query.begin(), query.end(), IsEqual) != text.end();
        }
    }

    /* --- LOG STORE --- */

    class LogStore final
    {
    public:
        void Append(LogPanelEntry entry)
        {
            const std::scoped_lock lock(mutex);

            const std::string key = MakeMessageKey(entry.message, entry.severity);
            const auto found = index.find(key);
            if (found != index.end())
            {
                // A repeat is counted and moved to the end, so the list stays ordered by last write
                found->second->count++;
                found->second->timePoint = entry.timePoint;
                entries.splice(entries.end(), entries, found->second);
            }
            else
            {
                entry.ID = ++lastId;
                entries.emplace_back(std::move(entry));
                index.emplace(key, std::prev(entries.end()));

                if (entries.size() > MAX_DISTINCT_ENTRY_COUNT)
                {
                    index.erase(MakeMessageKey(entries.front().message, entries.front().severity));
                    entries.pop_front();
                }
            }

            version++;
        }

        // Copies the list into the destination if it changed since knownVersion, and updates knownVersion
        bool CopyIfChanged(uint64& knownVersion, std::vector<LogPanelEntry>& destination) const
        {
            const std::scoped_lock lock(mutex);
            if (knownVersion == version) return false;

            destination.assign(entries.begin(), entries.end());
            knownVersion = version;
            return true;
        }

        void Clear()
        {
            const std::scoped_lock lock(mutex);

            entries.clear();
            index.clear();
            version++;
        }

    private:
        mutable std::mutex mutex = { };
        std::list<LogPanelEntry> entries = { };
        std::unordered_map<std::string, std::list<LogPanelEntry>::iterator> index = { };
        uint64 lastId = 0;
        uint64 version = 0;

    };

    class LogPanel::LogPanelSink final : public spdlog::sinks::base_sink<std::mutex>
    {
    public:
        LogPanelSink()
            : store(std::make_shared<LogStore>())
        {

        }

        [[nodiscard]] LogStore& GetStore() const noexcept { return *store; }

    protected:
        void sink_it_(const spdlog::details::log_msg& message) override
        {
            LogPanelEntry entry { };
            entry.timePoint = Sierra::TimePoint(message.time);
            entry.severity = SpdlogLevelToSeverity(message.level);
            entry.message.assign(message.payload.data(), message.payload.size());

            store->Append(std::move(entry));
        }

        void flush_() override
        {

        }

    private:
        std::shared_ptr<LogStore> store = { };

    };

    LogPanel::LogPanel(const LogPanelCreateInfo& createInfo)
        : sink(std::make_shared<LogPanelSink>())
    {
        // Attached once and never removed, so the logger's sink list is only changed here
        createInfo.logger.sinks().emplace_back(sink);
    }

    /* --- POLLING METHODS --- */

    void LogPanel::Draw()
    {
        SyncEntries();

        // Items in the panel get the window padding as their vertical spacing, so separators have the same space above and below them
        if (ImGui::Begin(PANEL_TITLE.data(), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoNav))
        {
            DrawToolBar();
            if (visibleEntriesDirty) RefreshVisibleEntries();

            ImGui::Separator();

            const ImGuiStyle& style = ImGui::GetStyle();
            const float32 statusBarHeight = ImGui::GetTextLineHeightWithSpacing() + style.ItemSpacing.y;
            const float32 bodyHeight = glm::max(0.0f, ImGui::GetContentRegionAvail().y - statusBarHeight);
            const ImVec2 bodyStart = ImGui::GetCursorScreenPos();
            const float32 detailsHeight = std::clamp(bodyHeight * DETAILS_HEIGHT_RATIO, DETAILS_MIN_HEIGHT, DETAILS_MAX_HEIGHT);

            if (visibleEntryIndices.empty()) DrawEmptyEntries(bodyHeight);
            else DrawEntries(bodyHeight);

            const LogPanelEntry* selectedEntry = FindSelectedEntry();
            if (selectedEntry != nullptr)
            {
                ImGui::SetCursorScreenPos({ bodyStart.x, bodyStart.y + bodyHeight - detailsHeight });
                DrawDetails(*selectedEntry, detailsHeight);
            }

            ImGui::SetCursorScreenPos({ bodyStart.x, bodyStart.y + bodyHeight });
            DrawStatusBar();
        }
        ImGui::End();
    }

    void LogPanel::Clear()
    {
        sink->GetStore().Clear();
        entries.clear();

        selectedId = 0;
        visibleEntriesDirty = true;
    }

    /* --- PRIVATE METHODS --- */

    void LogPanel::SyncEntries()
    {
        if (sink->GetStore().CopyIfChanged(syncedVersion, entries)) visibleEntriesDirty = true;
    }

    void LogPanel::RefreshVisibleEntries()
    {
        severityCounts = { };
        visibleEntryIndices.clear();

        const bool searching = !searchText.empty();
        for (size i = 0; i < entries.size(); i++)
        {
            const LogPanelEntry& entry = entries[i];

            // Counts are totals, so a message repeated ten times adds ten to its severity
            const size severityIndex = static_cast<size>(entry.severity);
            severityCounts[severityIndex] += static_cast<size>(entry.count);

            if (!severityFilters[severityIndex]) continue;
            if (searching && !ContainsIgnoreCase(entry.message, searchText)) continue;

            visibleEntryIndices.emplace_back(i);
        }

        visibleEntriesDirty = false;
    }

    const LogPanelEntry* LogPanel::FindSelectedEntry() const noexcept
    {
        if (selectedId == 0) return nullptr;

        const uint64 ID = selectedId;
        const auto iterator = std::find_if(entries.begin(), entries.end(), [ID](const LogPanelEntry& entry) -> bool
        {
            return entry.ID == ID;
        });

        return iterator != entries.end() ? &*iterator : nullptr;
    }

    void LogPanel::DrawToolBar()
    {
        const ImGuiStyle& style = ImGui::GetStyle();

        if (ImGui::Button("Clear")) Clear();

        ImGui::SameLine();
        if (ImGui::Checkbox("Auto-scroll", &autoScroll) && autoScroll) pendingScroll = true;

        ImGui::SameLine();
        if (ImGuiWidgets::TextInput("##SearchInput", searchText, { .width = SEARCH_WIDTH })) visibleEntriesDirty = true;

        std::array<std::string, SEVERITY_COUNT> labels = { };
        float32 togglesWidth = 0.0f;
        for (size i = 0; i < SEVERITY_COUNT; i++)
        {
            labels[i] = SR_FORMAT("{0} ({1})##Severity{2}", SeverityToString(static_cast<LogPanelSeverity>(i)), severityCounts[i], i);
            togglesWidth += ImGui::CalcTextSize(labels[i].c_str(), nullptr, true).x + style.FramePadding.x * 2.0f + style.ItemSpacing.x;
        }

        const auto ToggleButton = [](const char* label, const bool active) -> bool
        {
            if (active) ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive));
            const bool pressed = ImGui::Button(label);
            if (active) ImGui::PopStyleColor();
            return pressed;
        };

        ImGui::SameLine(std::max(ImGui::GetCursorPosX() + style.ItemSpacing.x, ImGui::GetContentRegionMax().x - togglesWidth));
        for (size i = 0; i < SEVERITY_COUNT; i++)
        {
            if (i > 0) ImGui::SameLine();
            if (ToggleButton(labels[i].c_str(), severityFilters[i]))
            {
                severityFilters[i] = !severityFilters[i];
                visibleEntriesDirty = true;
            }
        }
    }

    void LogPanel::DrawEntries(const float32 height)
    {
        constexpr ImGuiTableFlags TABLE_FLAGS = ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingStretchProp;
        if (!ImGui::BeginTable("##LogEntries", 3, TABLE_FLAGS, ImVec2(0.0f, height))) return;

        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, TIME_COLUMN_WIDTH);
        ImGui::TableSetupColumn("Severity", ImGuiTableColumnFlags_WidthFixed, SEVERITY_COLUMN_WIDTH);
        ImGui::TableSetupColumn("Message", ImGuiTableColumnFlags_WidthStretch, 1.0f);
        ImGui::TableHeadersRow();

        // Only follow new messages while the list is already scrolled to its end, so that reading older ones is not interrupted
        const bool atBottom = ImGui::GetScrollY() >= ImGui::GetScrollMaxY();

        ImGuiListClipper clipper;
        clipper.Begin(static_cast<int>(visibleEntryIndices.size()));
        while (clipper.Step())
        {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
            {
                DrawEntry(entries[visibleEntryIndices[static_cast<size>(row)]]);
            }
        }

        if (autoScroll && (atBottom || pendingScroll)) ImGui::SetScrollHereY(1.0f);
        pendingScroll = false;

        // Clicks on empty space clear the selection
        if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) selectedId = 0;

        ImGui::EndTable();
    }

    void LogPanel::DrawEmptyEntries(const float32 height)
    {
        if (ImGui::BeginChild("##LogEntries", ImVec2(0.0f, height), ImGuiChildFlags_Borders))
        {
            const bool noEntries = entries.empty();
            const std::string_view message = noEntries ? "No messages have been logged" : "No messages match the current filters";
            const std::string_view hint = noEntries ? "Messages logged through Sierra::Logger appear here" : "Clear the search or enable a severity to see more";

            const ImVec2 available = ImGui::GetContentRegionAvail();
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + available.y * 0.35f);

            ImGui::SetCursorPosX(std::max(0.0f, (available.x - ImGui::CalcTextSize(message.data(), message.data() + message.size()).x) * 0.5f));
            ImGui::TextUnformatted(message.data(), message.data() + message.size());

            ImGui::SetCursorPosX(std::max(0.0f, (available.x - ImGui::CalcTextSize(hint.data(), hint.data() + hint.size()).x) * 0.5f));
            ImGui::TextDisabled("%.*s", static_cast<int>(hint.size()), hint.data());
        }
        ImGui::EndChild();
    }

    void LogPanel::DrawEntry(const LogPanelEntry& entry)
    {
        ImGui::TableNextRow();

        ImGui::TableNextColumn();

        // Time is the label of the selectable, which spans the whole row
        const std::string timeLabel = FormatTime(entry.timePoint);
        const std::string selectableLabel = SR_FORMAT("{0}##{1}", timeLabel, entry.ID);
        if (ImGui::Selectable(selectableLabel.c_str(), entry.ID == selectedId, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap))
        {
            // Clicking the selected row again deselects it
            selectedId = entry.ID == selectedId ? 0 : entry.ID;
        }
        

        if (entry.severity == LogPanelSeverity::Warning || entry.severity == LogPanelSeverity::Error)
        {
            ImVec4 highlightColor = SeverityToColor(entry.severity);
            highlightColor.w = ROW_HIGHLIGHT_ALPHA;
            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImGui::GetColorU32(highlightColor));
        }

        ImGui::TableNextColumn();
        ImGui::TextColored(SeverityToColor(entry.severity), "%s", SeverityToString(entry.severity).data());

        ImGui::TableNextColumn();
        const std::string summary = GetSummary(entry.message);
        ImGui::TextUnformatted(summary.c_str(), summary.c_str() + summary.size());

        if (entry.count > 1) DrawCountPill(entry.count);
    }

    void LogPanel::DrawDetails(const LogPanelEntry& entry, const float32 height)
    {
        // The details are drawn over the list, so they need an opaque background
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyleColorVec4(ImGuiCol_WindowBg));
        if (ImGui::BeginChild("##LogDetails", ImVec2(0.0f, height), ImGuiChildFlags_Borders))
        {
            const ImGuiStyle& style = ImGui::GetStyle();

            // The cross at the top left closes the details, which deselects the entry
            if (DrawCrossButton("##CloseDetails") || ImGui::IsKeyPressed(ImGuiKey_Escape)) selectedId = 0;
            ImGui::SameLine();

            // Puts the date line's text in the middle of the cross's frame, as the cross is drawn there
            ImGui::AlignTextToFramePadding();
            const std::string dateLabel = Sierra::TimePointToString(entry.timePoint);
            ImGui::TextDisabled("%s", dateLabel.c_str());
            ImGui::SameLine();
            ImGui::TextColored(SeverityToColor(entry.severity), "%s", SeverityToString(entry.severity).data());

            // Logger name and copy button share the second line
            ImGui::SameLine();
            const std::string loggerName = Sierra::Logger::GetApplicationLogger().name();
            ImGui::TextDisabled("| Logger: %s", loggerName.c_str());

            // Copy and Close sit at the right of the line, Close last
            const float32 copyWidth = ImGui::CalcTextSize("Copy").x + style.FramePadding.x * 2.0f;
            const float32 closeWidth = ImGui::CalcTextSize("Close").x + style.FramePadding.x * 2.0f;
            ImGui::SameLine(std::max(ImGui::GetCursorPosX() + style.ItemSpacing.x, ImGui::GetContentRegionMax().x - (copyWidth + style.ItemSpacing.x + closeWidth)));
            if (ImGui::Button("Copy")) ImGui::SetClipboardText(entry.message.c_str());
            ImGui::SameLine();
            if (ImGui::Button("Close")) selectedId = 0;

            ImGui::Separator();
            ImGui::TextWrapped("%s", entry.message.c_str());
        }
        ImGui::EndChild();
        ImGui::PopStyleColor();
    }

    void LogPanel::DrawStatusBar()
    {
        ImGui::Separator();

        ImGui::TextDisabled("%zu of %zu distinct messages", visibleEntryIndices.size(), entries.size());
    }

}
