target_sources(SierraEngineEditor PRIVATE
    Editor.cpp
    Editor.h
    EditorThemes.cpp
    EditorThemes.h
    EditorWizard.h
    EditorWizard.cpp
)

add_subdirectory(Panels)
add_subdirectory(Previews)
add_subdirectory(Widgets)
add_subdirectory(Wizards)