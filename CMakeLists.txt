cmake_minimum_required(VERSION 3.16)

project(lingmo-texteditor LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QT Core Gui Widgets Qml Quick QuickControls2 LinguistTools)
find_package(Qt6 REQUIRED COMPONENTS ${QT})
find_package(KF6SyntaxHighlighting REQUIRED)

set(PROJECT_SOURCES
        src/main.cpp
        src/documenthandler.cpp
        src/highlightmodel.cpp
        src/texteditor.cpp
        qml.qrc
)

add_executable(lingmo-texteditor
  ${PROJECT_SOURCES}
)

target_link_libraries(lingmo-texteditor
  PRIVATE
  Qt6::Core
  Qt6::Gui
  Qt6::Widgets
  Qt6::Qml
  Qt6::Quick
  Qt6::QuickControls2
  KF6::SyntaxHighlighting
  )

file(GLOB TS_FILES translations/*.ts)
qt_add_translation(QM_FILES ${TS_FILES})
add_custom_target(translations ALL DEPENDS ${QM_FILES} SOURCES ${TS_FILES})
  
install(TARGETS lingmo-texteditor RUNTIME DESTINATION /usr/bin)
install(FILES ${QM_FILES} DESTINATION /usr/share/lingmo-texteditor/translations)

install(FILES
    lingmo-texteditor.desktop
    DESTINATION /usr/share/applications/
    COMPONENT Runtime
)

# Local notebook, shipped alongside the text editor.
add_executable(lingmo-notes
  notes/main.cpp
  notes/markdown.cpp notes/markdown.h
  notes/blockmodel.cpp notes/blockmodel.h
  notes/pagetreemodel.cpp notes/pagetreemodel.h
  notes/notestore.cpp notes/notestore.h
  notes/notes.qrc)
target_link_libraries(lingmo-notes PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Qml Qt6::Quick Qt6::QuickControls2)
install(TARGETS lingmo-notes RUNTIME DESTINATION bin)
install(FILES lingmo-notes.desktop DESTINATION share/applications)

# Lingmo Intents: notes.create / notes.append on com.lingmo.Notes /Intents
# (D-Bus activation starts `lingmo-notes --intents` without a window)
find_package(LingmoIntents 1.0 QUIET)
if(LingmoIntents_FOUND)
  target_sources(lingmo-notes PRIVATE notes/notesintents.cpp notes/notesintents.h)
  target_compile_definitions(lingmo-notes PRIVATE HAVE_LINGMO_INTENTS)
  target_link_libraries(lingmo-notes PRIVATE LingmoIntents::LingmoIntents)
  include(GNUInstallDirs)
  lingmo_install_intents(MANIFEST data/intents/lingmo-notes.json SERVICE data/com.lingmo.Notes.service)
else()
  message(WARNING "LingmoIntents not found: Notes is built without app actions (intents)")
endif()
