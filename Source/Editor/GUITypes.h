//
// Created by Nikolay Kanchevski on 21.07.23.
//

#pragma once

namespace Sierra::Editor
{
    enum class WindowFlags
    {
        NONE = ImGuiWindowFlags_None,
        NO_TITLE_BAR = ImGuiWindowFlags_NoTitleBar,
        NO_RESIZE = ImGuiWindowFlags_NoResize,
        NO_MOVE = ImGuiWindowFlags_NoMove,
        NO_SCROLLBAR = ImGuiWindowFlags_NoScrollbar,
        NO_MOUSE_SCROLL = ImGuiWindowFlags_NoScrollWithMouse,
        NO_COLLAPSE = ImGuiWindowFlags_NoCollapse,
        ALWAYS_AUTO_RESIZE = ImGuiWindowFlags_AlwaysAutoResize,
        NO_BACKGROUND = ImGuiWindowFlags_NoBackground,
        NO_SAVED_SETTINGS = ImGuiWindowFlags_NoSavedSettings,
        NO_MOUSE_INPUTS = ImGuiWindowFlags_NoMouseInputs,
        MENU_BAR = ImGuiWindowFlags_MenuBar,
        SHOW_HORIZONTAL_SCROLLBAR = ImGuiWindowFlags_HorizontalScrollbar,
        NO_FOCUS_ON_APPEARING = ImGuiWindowFlags_NoFocusOnAppearing,
        NO_BRING_TO_FRONT_ON_FOCUS = ImGuiWindowFlags_NoBringToFrontOnFocus,
        ALWAYS_SHOW_VERTICAL_SCROLLBAR = ImGuiWindowFlags_AlwaysVerticalScrollbar,
        ALWAYS_SHOW_HORIZONTAL_SCROLLBAR = ImGuiWindowFlags_AlwaysHorizontalScrollbar,
        ALWAYS_USE_WINDOW_PADDING = ImGuiWindowFlags_AlwaysUseWindowPadding,
        NO_NAV_INPUTS = ImGuiWindowFlags_NoNavInputs,
        NO_NAV_FOCUS = ImGuiWindowFlags_NoNavFocus,
        UNSAVED_DOCUMENT_STATE = ImGuiWindowFlags_UnsavedDocument,
        NO_DOCKING = ImGuiWindowFlags_NoDocking,
        NO_NAV = ImGuiWindowFlags_NoNav,
        NO_DECORATION = ImGuiWindowFlags_NoDecoration,
        NO_INPUTS = ImGuiWindowFlags_NoInputs
    };
    DEFINE_ENUM_FLAG_OPERATORS(WindowFlags);

    enum class TreeFlags
    {
        NONE = ImGuiTreeNodeFlags_None,
        SELECTED = ImGuiTreeNodeFlags_Selected,
        FRAMED = ImGuiTreeNodeFlags_Framed,
        ALLOW_ITEM_OVERLAP = ImGuiTreeNodeFlags_AllowItemOverlap,
        NO_TREE_PUSH_ON_OPEN = ImGuiTreeNodeFlags_NoTreePushOnOpen,
        NO_AUTO_OPEN_ON_LOG = ImGuiTreeNodeFlags_NoAutoOpenOnLog,
        OPEN_BY_DEFAULT = ImGuiTreeNodeFlags_DefaultOpen,
        OPEN_ON_DOUBLE_CLICK = ImGuiTreeNodeFlags_OpenOnDoubleClick,
        OPEN_ON_ARROW = ImGuiTreeNodeFlags_OpenOnArrow,
        LEAF = ImGuiTreeNodeFlags_Leaf,
        BULLET = ImGuiTreeNodeFlags_Bullet,
        FRAME_PADDING = ImGuiTreeNodeFlags_FramePadding,
        SPAN_AVAILABLE_WIDTH = ImGuiTreeNodeFlags_SpanAvailWidth,
        SPAN_FULL_WIDTH = ImGuiTreeNodeFlags_SpanFullWidth,
        NAV_LEFT_JUMPS_BACK_HERE = ImGuiTreeNodeFlags_NavLeftJumpsBackHere,
        COLLAPSING_HEADER = ImGuiTreeNodeFlags_CollapsingHeader
    };
    DEFINE_ENUM_FLAG_OPERATORS(TreeFlags);

    enum class StyleVariable
    {
        ALPHA = ImGuiStyleVar_Alpha,
        DISABLED_ALPHA = ImGuiStyleVar_DisabledAlpha,
        WINDOW_PADDING = ImGuiStyleVar_WindowPadding,
        WINDOW_ROUNDING = ImGuiStyleVar_WindowRounding,
        WINDOW_BORDER_SIZE = ImGuiStyleVar_WindowBorderSize,
        WINDOW_MINIMUM_SIZE = ImGuiStyleVar_WindowMinSize,
        WINDOW_TITLE_ALIGN = ImGuiStyleVar_WindowTitleAlign,
        CHILD_ROUNDING = ImGuiStyleVar_ChildRounding,
        CHILD_BORDER_SIZE = ImGuiStyleVar_ChildBorderSize,
        POPUP_ROUNDING = ImGuiStyleVar_PopupRounding,
        POPUP_BORDER_SIZE = ImGuiStyleVar_PopupBorderSize,
        FRAME_PADDING = ImGuiStyleVar_FramePadding,
        FRAME_ROUNDING = ImGuiStyleVar_FrameRounding,
        FRAME_BORDER_SIZE = ImGuiStyleVar_FrameBorderSize,
        ITEM_SPACING = ImGuiStyleVar_ItemSpacing,
        INNER_ITEM_SPACING = ImGuiStyleVar_ItemInnerSpacing,
        INDENT_SPACING = ImGuiStyleVar_IndentSpacing,
        CELL_PADDING = ImGuiStyleVar_CellPadding,
        SCROLLBAR_SIZE = ImGuiStyleVar_ScrollbarSize,
        SCROLLBAR_ROUNDING = ImGuiStyleVar_ScrollbarRounding,
        MINIMUM_GRAB_SIZE = ImGuiStyleVar_GrabMinSize,
        GRAB_ROUNDING = ImGuiStyleVar_GrabRounding,
        TAB_ROUNDING = ImGuiStyleVar_TabRounding,
        BUTTON_TEXT_ALIGN = ImGuiStyleVar_ButtonTextAlign,
        SELECTABLE_TEXT_ALIGN = ImGuiStyleVar_SelectableTextAlign,
        SEPARATOR_TEXT_BORDER_SIZE = ImGuiStyleVar_SeparatorTextBorderSize,
        SEPARATOR_TEXT_ALIGN = ImGuiStyleVar_SeparatorTextAlign,
        SEPARATOR_TEXT_PADDING = ImGuiStyleVar_SeparatorTextPadding
    };
    DEFINE_ENUM_FLAG_OPERATORS(StyleVariable);

    enum class StyleColor
    {
        TEXT = ImGuiCol_Text,
        DISABLED_TEXT = ImGuiCol_TextDisabled,
        WINDOW_BACKGROUND = ImGuiCol_WindowBg,
        CHILD_BACKGROUND = ImGuiCol_ChildBg,
        POPUP_BACKGROUND = ImGuiCol_PopupBg,
        BORDER = ImGuiCol_Border,
        BORDER_SHADOW = ImGuiCol_BorderShadow,
        FRAME_BACKGROUND = ImGuiCol_FrameBg,
        HOVERED_FRAME_BACKGROUND = ImGuiCol_FrameBgHovered,
        ACTIVE_FRAME_BACKGROUND = ImGuiCol_FrameBgActive,
        TITLE_BACKGROUND = ImGuiCol_TitleBg,
        ACTIVE_TITLE_BACKGROUND = ImGuiCol_TitleBgActive,
        COLLAPSED_TITLE_BACKGROUND = ImGuiCol_TitleBgCollapsed,
        MENU_BAR_BACKGROUND = ImGuiCol_MenuBarBg,
        SCROLLBAR_BACKGROUND = ImGuiCol_ScrollbarBg,
        SCROLLBAR_GRAB = ImGuiCol_ScrollbarGrab,
        HOVERED_SCROLLBAR_GRAB = ImGuiCol_ScrollbarGrabHovered,
        ACTIVE_SCROLLBAR_GRAB = ImGuiCol_ScrollbarGrabActive,
        CHECK_MARK = ImGuiCol_CheckMark,
        SLIDER_GRAB = ImGuiCol_SliderGrab,
        ACTIVE_SLIDER_GRAB = ImGuiCol_SliderGrabActive,
        BUTTON = ImGuiCol_Button,
        HOVERED_BUTTON = ImGuiCol_ButtonHovered,
        ACTIVE_BUTTON = ImGuiCol_ButtonActive,
        HEADER = ImGuiCol_Header,
        HOVERED_HEADER = ImGuiCol_HeaderHovered,
        ACTIVE_HEADER = ImGuiCol_HeaderActive,
        SEPARATOR = ImGuiCol_Separator,
        HOVERED_SEPARATOR = ImGuiCol_SeparatorHovered,
        ACTIVE_SEPARATOR = ImGuiCol_SeparatorActive,
        RESIZE_GRIP = ImGuiCol_ResizeGrip,
        HOVERED_RESIZE_GRIP = ImGuiCol_ResizeGripHovered,
        ACTIVE_RESIZE_GRIP = ImGuiCol_ResizeGripActive,
        TAB = ImGuiCol_Tab,
        HOVERED_TAB = ImGuiCol_TabHovered,
        ACTIVE_TAB = ImGuiCol_TabActive,
        UNFOCUSED_TAB = ImGuiCol_TabUnfocused,
        UNFOCUSED_ACTIVE_TAB = ImGuiCol_TabUnfocusedActive,
        DOCKING_PREVIEW = ImGuiCol_DockingPreview,
        DOCKING_EMPTY_BACKGROUND = ImGuiCol_DockingEmptyBg,
        PLOT_LINES = ImGuiCol_PlotLines,
        HOVERED_PLOT_LINES = ImGuiCol_PlotLinesHovered,
        PLOT_HISTOGRAM = ImGuiCol_PlotHistogram,
        HOVERED_PLOT_HISTOGRAM = ImGuiCol_PlotHistogramHovered,
        TABLE_HEADER_BACKGROUND = ImGuiCol_TableHeaderBg,
        STRONG_TABLE_BORDER = ImGuiCol_TableBorderStrong,
        LIGHT_TABLE_BORDER = ImGuiCol_TableBorderLight,
        TABLE_ROW_BACKGROUND = ImGuiCol_TableRowBg,
        ALT_TABLE_ROW_BACKGROUND = ImGuiCol_TableRowBgAlt,
        SELECTED_TEXT_BACKGROUND = ImGuiCol_TextSelectedBg,
        DRAG_DROP_TARGET = ImGuiCol_DragDropTarget,
        NAV_HIGHLIGHT = ImGuiCol_NavHighlight,
        NAV_WINDOWING_HIGHLIGHT = ImGuiCol_NavWindowingHighlight,
        NAV_WINDOWING_DIM_BACKGROUND = ImGuiCol_NavWindowingDimBg,
        MODAL_WINDOW_DIM_BACKGROUND = ImGuiCol_ModalWindowDimBg
    };
    DEFINE_ENUM_FLAG_OPERATORS(StyleColor);

    enum class InputFlags
    {
        NONE = ImGuiInputTextFlags_None,
        DECIMAL_CHARACTERS = ImGuiInputTextFlags_CharsDecimal,
        HEXADECIMAL_CHARACTERS = ImGuiInputTextFlags_CharsHexadecimal,
        UPPERCASE_CHARACTERS = ImGuiInputTextFlags_CharsUppercase,
        NO_BLANK_CHARACTERS = ImGuiInputTextFlags_CharsNoBlank,
        AUTO_SELECT_ALL = ImGuiInputTextFlags_AutoSelectAll,
        ENTER_RETURNS_TRUE = ImGuiInputTextFlags_EnterReturnsTrue,
        CALLBACK_COMPLETION = ImGuiInputTextFlags_CallbackCompletion,
        CALLBACK_HISTORY = ImGuiInputTextFlags_CallbackHistory,
        CALLBACK_ALWAYS = ImGuiInputTextFlags_CallbackAlways,
        CALLBACK_CHARACTER_FILTER = ImGuiInputTextFlags_CallbackCharFilter,
        ALLOW_TAB_INPUT = ImGuiInputTextFlags_AllowTabInput,
        CTRL_ENTER_FOR_NEW_LINE = ImGuiInputTextFlags_CtrlEnterForNewLine,
        NO_HORIZONTAL_SCROLL = ImGuiInputTextFlags_NoHorizontalScroll,
        ALWAYS_OVERWRITE = ImGuiInputTextFlags_AlwaysOverwrite,
        READ_ONLY = ImGuiInputTextFlags_ReadOnly,
        PASSWORD = ImGuiInputTextFlags_Password,
        NO_UNDO_OR_REDO = ImGuiInputTextFlags_NoUndoRedo,
        SCIENTIFIC_CHARACTERS = ImGuiInputTextFlags_CharsScientific,
        CALLBACK_RESIZE = ImGuiInputTextFlags_CallbackResize,
        CALLBACK_EDIT = ImGuiInputTextFlags_CallbackEdit,
        ESCAPE_CLEARS_ALL = ImGuiInputTextFlags_EscapeClearsAll,

        // [Internal] - Not supposed to be used by the user!
        MULTILINE = ImGuiInputTextFlags_Multiline,
        NO_MARK_EDITED = ImGuiInputTextFlags_NoMarkEdited,
        MERGED_ITEM = ImGuiInputTextFlags_MergedItem
    };
    DEFINE_ENUM_FLAG_OPERATORS(InputFlags);

    enum class CornerRoundingFlags
    {
        DEFAULT = ImDrawFlags_RoundCornersDefault_,
        NONE = ImDrawFlags_RoundCornersNone,
        TOP_LEFT = ImDrawFlags_RoundCornersTopLeft,
        TOP_RIGHT = ImDrawFlags_RoundCornersTopRight,
        BOTTOM_LEFT = ImDrawFlags_RoundCornersBottomLeft,
        BOTTOM_RIGHT = ImDrawFlags_RoundCornersBottomRight,
        TOP = ImDrawFlags_RoundCornersTop,
        BOTTOM = ImDrawFlags_RoundCornersBottom,
        LEFT = ImDrawFlags_RoundCornersLeft,
        RIGHT = ImDrawFlags_RoundCornersRight,
        ALL = ImDrawFlags_RoundCornersAll
    };
}
