target_sources(SierraEngineEditor PRIVATE
    Editor.cpp
    Editor.h
    EditorModal.h
    EditorModal.cpp
    EditorThemes.cpp
    EditorThemes.h
)

add_subdirectory(Dialogs)
add_subdirectory(Panels)
add_subdirectory(Previews)
add_subdirectory(Widgets)
add_subdirectory(Wizards)
