cmake_minimum_required(VERSION 3.16)
project(notes-tests LANGUAGES CXX)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(Qt6 REQUIRED COMPONENTS Core Test)
enable_testing()
add_executable(notes-test
    notestoretest.cpp
    ../markdown.cpp ../markdown.h
    ../blockmodel.cpp ../blockmodel.h
    ../pagetreemodel.cpp ../pagetreemodel.h
    ../notestore.cpp ../notestore.h)
target_include_directories(notes-test PRIVATE ..)
target_link_libraries(notes-test PRIVATE Qt6::Core Qt6::Test)
add_test(NAME notes COMMAND notes-test)
