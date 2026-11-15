//
// Created by Nikolay Kanchevski on 15.11.26.
//

#pragma once

#include <array>
#include <vector>

namespace SierraEngine
{

    enum class LogPanelSeverity : uint8
    {
        Debug,
        Info,
        Warning,
        Error
    };

    struct LogPanelEntry
    {
        uint64 ID = 0;
        uint64 count = 1;
        Sierra::TimePoint timePoint = { };
        LogPanelSeverity severity = LogPanelSeverity::Info;
        std::string message = { };
    };

    struct LogPanelCreateInfo
    {
        spdlog::logger& logger;
    };

    class LogPanel final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit LogPanel(const LogPanelCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        void Draw();

        void Clear();

        /* --- COPY SEMANTICS --- */
        LogPanel(const LogPanel&) = delete;
        LogPanel& operator=(const LogPanel&) = delete;

        /* --- MOVE SEMANTICS --- */
        LogPanel(LogPanel&&) noexcept = default;
        LogPanel& operator=(LogPanel&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~LogPanel() noexcept = default;

    private:
        class LogPanelSink;
        std::shared_ptr<LogPanelSink> sink = nullptr;

        std::vector<LogPanelEntry> entries = { };
        uint64 syncedVersion = 0;
        uint64 selectedId = 0;

        std::vector<size> visibleEntryIndices = { };
        static constexpr size SEVERITY_COUNT = static_cast<size>(LogPanelSeverity::Error) + 1;
        std::array<size, SEVERITY_COUNT> severityCounts = { };
        std::array<bool, SEVERITY_COUNT> severityFilters = { true, true, true, true };

        std::string searchText = { };
        bool visibleEntriesDirty = true;

        bool autoScroll = true;
        bool pendingScroll = false;

        void SyncEntries();
        void RefreshVisibleEntries();
        [[nodiscard]] const LogPanelEntry* FindSelectedEntry() const noexcept;

        void DrawToolBar();
        void DrawEntries(float32 height);
        void DrawEmptyEntries(float32 height);
        void DrawEntry(const LogPanelEntry& entry);
        void DrawDetails(const LogPanelEntry& entry, float32 height);
        void DrawStatusBar();

    };

}
