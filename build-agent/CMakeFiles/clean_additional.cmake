# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "CMakeFiles/lingmo-notes_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/lingmo-notes_autogen.dir/ParseCache.txt"
  "CMakeFiles/lingmo-texteditor_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/lingmo-texteditor_autogen.dir/ParseCache.txt"
  "lingmo-notes_autogen"
  "lingmo-texteditor_autogen"
  )
endif()
